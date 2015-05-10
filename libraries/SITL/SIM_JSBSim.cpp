/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
  simulator connector for ardupilot version of JSBSim
*/

#include <AP_HAL.h>
#if CONFIG_HAL_BOARD == HAL_BOARD_SITL

#include "SIM_JSBSim.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>

extern const AP_HAL::HAL& hal;

#pragma GCC diagnostic ignored "-Wunused-result"

/*
  constructor
 */
JSBSim::JSBSim(const char *home_str, const char *frame_str) :
    Aircraft(home_str, frame_str),
    sock_control(false),
    sock_fgfdm(true),
    initialised(false),
    jsbsim_script(NULL),
    jsbsim_fgout(NULL),
    created_templates(false),
    started_jsbsim(false),
    opened_control_socket(false),
    opened_fdm_socket(false)
{
}

/*
  create template files
 */
bool JSBSim::create_templates(void)
{
    if (created_templates) {
        return true;
    }
    control_port = 5505 + instance*10;
    fdm_port = 5504 + instance*10;


    asprintf(&autotest_dir, SKETCHBOOK "/Tools/autotest");
    asprintf(&jsbsim_script, "%s/jsbsim_start_%u.xml", autotest_dir, instance);
    asprintf(&jsbsim_fgout,  "%s/jsbsim_fgout_%u.xml", autotest_dir, instance);

    FILE *f = fopen(jsbsim_script, "w");
    if (f == NULL) {
        hal.scheduler->panic("Unable to create jsbsim script");
    }
    fprintf(f,
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<?xml-stylesheet type=\"text/xsl\" href=\"http://jsbsim.sf.net/JSBSimScript.xsl\"?>\n"
"<runscript xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
"    xsi:noNamespaceSchemaLocation=\"http://jsbsim.sf.net/JSBSimScript.xsd\"\n"
"    name=\"Testing Rascal110\">\n"
"\n"
"  <description>\n"
"    test ArduPlane using Rascal110 and JSBSim\n"
"  </description>\n"
"\n"
"  <use aircraft=\"Rascal\" initialize=\"reset\"/>\n"
"\n"
"  <!-- we control the servos via the jsbsim console\n"
"       interface on TCP 5124 -->\n"
"  <input port=\"%u\"/>\n"
"\n"
"  <run start=\"0\" end=\"10000000\" dt=\"0.001\">\n"
"    <property value=\"0\"> simulation/notify-time-trigger </property>\n"
"\n"
"    <event name=\"start engine\">\n"
"      <condition> simulation/sim-time-sec le 0.01 </condition>\n"
"      <set name=\"propulsion/engine[0]/set-running\" value=\"1\"/>\n"
"      <notify/>\n"
"    </event>\n"
"\n"
"    <event name=\"Trim\">\n"
"      <condition>simulation/sim-time-sec ge 0.01</condition>\n"
"      <set name=\"simulation/do_simple_trim\" value=\"2\"/>\n"
"      <notify/>\n"
"    </event>\n"
"  </run>\n"
"\n"
"</runscript>\n"
"", control_port);
    fclose(f);

    f = fopen(jsbsim_fgout, "w");
    if (f == NULL) {
        hal.scheduler->panic("Unable to create jsbsim fgout script");
    }
    fprintf(f, "<?xml version=\"1.0\"?>\n" 
            "<output name=\"localhost\" type=\"FLIGHTGEAR\" port=\"%u\" protocol=\"udp\" rate=\"1000\"/>\n",
            fdm_port);
    fclose(f);

    created_templates = true;
    return true;
}


/*
  start JSBSim child
 */
bool JSBSim::start_JSBSim(void)
{
    if (started_jsbsim) {
        return true;
    }
    if (!open_fdm_socket()) {
        return false;
    }

    int p[2];
    int devnull = open("/dev/null", O_RDWR);
    if (pipe(p) != 0) {
        hal.scheduler->panic("Unable to create pipe");        
    }
    pid_t child_pid = fork();
    if (child_pid == 0) {
        // in child
        dup2(devnull, 0);
        dup2(p[1], 1);
        close(p[0]);
        char *logdirective;
        char *script;

        asprintf(&logdirective, "--logdirectivefile=%s", jsbsim_fgout);
        asprintf(&script, "--script=%s", jsbsim_script);

        if (chdir(autotest_dir) != 0) {
            perror(autotest_dir);
            exit(1);
        }

        int ret = execlp("JSBSim", 
                         "JSBSim", 
                         "--realtime",
                         "--suspend",
                         "--nice",
                         "--simulation-rate=1000",
                         logdirective,
                         script,
                         NULL);
        if (ret != 0) {
            perror("JSBSim");
        }
        exit(1);
    }
    close(p[1]);
    jsbsim_stdout = p[0];

    // read startup to be sure it is running
    char c;
    if (read(jsbsim_stdout, &c, 1) != 1) {
        hal.scheduler->panic("Unable to start JSBSim");
    }

    if (!expect("JSBSim Execution beginning")) {
        hal.scheduler->panic("Failed to start JSBSim");        
    }
    if (!open_control_socket()) {
        hal.scheduler->panic("Failed to open JSBSim control socket");
    }

    fcntl(jsbsim_stdout, F_SETFL, fcntl(jsbsim_stdout, F_GETFL, 0) | O_NONBLOCK);

    started_jsbsim = true;
    check_stdout();
    return true;
}

/*
  check for stdout from JSBSim
 */
void JSBSim::check_stdout(void)
{
    char line[100];
    ssize_t ret = ::read(jsbsim_stdout, line, sizeof(line));
    if (ret > 0) {
        write(1, line, ret);
    }
}

/*
  a simple function to wait for a string on jsbsim_stdout
 */
bool JSBSim::expect(const char *str)
{
    const char *basestr = str;
    while (*str) {
        char c;        
        if (read(jsbsim_stdout, &c, 1) != 1) {
            return false;
        }
        if (c == *str) {
            str++;
        } else {
            str = basestr;
        }
        write(1, &c, 1);
    }
    return true;
}

/*
  open control socket to JSBSim
 */
bool JSBSim::open_control_socket(void)
{
    if (opened_control_socket) {
        return true;
    }
    if (!sock_control.connect("127.0.0.1", control_port)) {
        return false;
    }
    printf("Opened JSBSim control socket\n");
    sock_control.set_blocking(false);
    opened_control_socket = true;

    char startup[] = "info\nresume\nstep\n";
    sock_control.send(startup, strlen(startup));
    return true;
}

/*
  open fdm socket from JSBSim
 */
bool JSBSim::open_fdm_socket(void)
{
    if (opened_fdm_socket) {
        return true;
    }
    if (!sock_fgfdm.bind("127.0.0.1", fdm_port)) {
        check_stdout();
        return false;
    }
    sock_fgfdm.set_blocking(false);
    sock_fgfdm.reuseaddress();
    opened_fdm_socket = true;
    return true;
}


/*
  decode and send servos
*/
void JSBSim::send_servos(const struct sitl_input &input)
{
    char *buf = NULL;
    float aileron  = (input.servos[0]-1500)/500.0f;
    float elevator = (input.servos[1]-1500)/500.0f;
    float throttle = (input.servos[2]-1000)/1000.0;
    float rudder   = (input.servos[3]-1500)/500.0f;
    asprintf(&buf, 
             "set fcs/aileron-cmd-norm %f\n"
             "set fcs/elevator-cmd-norm %f\n"
             "set fcs/rudder-cmd-norm %f\n"
             "set fcs/throttle-cmd-norm %f\n"
             "step\n",
             aileron, elevator, rudder, throttle);
    sock_control.send(buf, strlen(buf));
    free(buf);
}

#define FEET_TO_METERS 0.3048f

// nasty hack ....
void FGNetFDM::ByteSwap(void) {
    uint32_t *buf = (uint32_t *)this;
    for (uint16_t i=0; i<sizeof(*this)/4; i++) {
        buf[i] = ntohl(buf[i]);
    }
    buf = (uint32_t *)&longitude;
    uint32_t tmp;
    for (uint8_t i=0; i<3; i++) {
        tmp = buf[0];
        buf[0] = buf[1];
        buf[1] = tmp;
        buf += 2;
    }
}

/*
  receive an update from the FDM
  This is a blocking function
 */
void JSBSim::recv_fdm(const struct sitl_input &input)
{
    FGNetFDM fdm;
    while (sock_fgfdm.recv(&fdm, sizeof(fdm), 100) != sizeof(fdm)) {
        send_servos(input);
        check_stdout();
    }
    fdm.ByteSwap();

    accel_body = Vector3f(fdm.A_X_pilot, fdm.A_Y_pilot, fdm.A_Z_pilot) * FEET_TO_METERS;

    double p, q, r;
    SITL::convert_body_frame(degrees(fdm.phi), degrees(fdm.theta),
                             degrees(fdm.phidot), degrees(fdm.thetadot), degrees(fdm.psidot),
                             &p, &q, &r);
    gyro = Vector3f(p, q, r);

    velocity_ef = Vector3f(fdm.v_north, fdm.v_east, fdm.v_down) * FEET_TO_METERS;
    location.lat = degrees(fdm.latitude) * 1.0e7;
    location.lng = degrees(fdm.longitude) * 1.0e7;
    location.alt = fdm.altitude*100;
    dcm.from_euler(fdm.phi, fdm.theta, fdm.psi);

    // assume 1kHz for now
    time_now_us += 1000;
}

/*
  update the JSBSim simulation by one time step
 */
void JSBSim::update(const struct sitl_input &input)
{
    while (!initialised) {
        if (!create_templates() ||
            !start_JSBSim() ||
            !open_control_socket() ||
            !open_fdm_socket()) {
            time_now_us = 1;
            return;
        }
        initialised = true;
    }
    send_servos(input);
    recv_fdm(input);
    adjust_frame_time(1000);
    sync_frame_time();
}
#endif // CONFIG_HAL_BOARD
