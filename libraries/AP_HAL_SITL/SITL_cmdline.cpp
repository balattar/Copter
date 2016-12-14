#include <AP_HAL/AP_HAL.h>

#if CONFIG_HAL_BOARD == HAL_BOARD_SITL

#include "AP_HAL_SITL.h"
#include "AP_HAL_SITL_Namespace.h"
#include "HAL_SITL_Class.h"
#include "UARTDriver.h"
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <AP_HAL/utility/getopt_cpp.h>

#include <SITL/SIM_Multicopter.h>
#include <SITL/SIM_Helicopter.h>
#include <SITL/SIM_SingleCopter.h>
#include <SITL/SIM_Plane.h>
#include <SITL/SIM_QuadPlane.h>
#include <SITL/SIM_Rover.h>
#include <SITL/SIM_CRRCSim.h>
#include <SITL/SIM_Gazebo.h>
#include <SITL/SIM_last_letter.h>
#include <SITL/SIM_JSBSim.h>
#include <SITL/SIM_Tracker.h>
#include <SITL/SIM_Balloon.h>
#include <SITL/SIM_FlightAxis.h>
#include <SITL/SIM_Calibration.h>
#include <SITL/SIM_XPlane.h>
#include <SITL/SIM_Submarine.h>

extern const AP_HAL::HAL& hal;

using namespace HALSITL;
using namespace SITL;

// catch floating point exceptions
static void _sig_fpe(int signum)
{
    fprintf(stderr, "ERROR: Floating point exception - aborting\n");
    abort();
}

void SITL_State::_usage(void)
{
    printf("Options:\n"
           "\t--home HOME              set home location (lat,lng,alt,yaw)\n"
           "\t--model MODEL            set simulation model\n"
           "\t--wipe                   wipe eeprom and dataflash\n"
           "\t--unhide-groups          parameter enumeration ignores AP_PARAM_FLAG_ENABLE\n"
           "\t--rate RATE              set SITL framerate\n"
           "\t--console                use console instead of TCP ports\n"
           "\t--instance N             set instance of SITL (adds 10*instance to all port numbers)\n"
           "\t--speedup SPEEDUP        set simulation speedup\n"
           "\t--gimbal                 enable simulated MAVLink gimbal\n"
           "\t--autotest-dir DIR       set directory for additional files\n"
           "\t--uartA device           set device string for UARTA\n"
           "\t--uartB device           set device string for UARTB\n"
           "\t--uartC device           set device string for UARTC\n"
           "\t--uartD device           set device string for UARTD\n"
           "\t--uartE device           set device string for UARTE\n"
           "\t--gazebo-address ADDR    set address string for gazebo\n"
           "\t--gazebo-port-in PORT    set port num for gazebo in\n"
           "\t--gazebo-port-out PORT   set port num for gazebo out\n"
           "\t--irlock-port PORT       set port num for irlock\n"
           "\t--rc-in-port PORT        set port num for rc in\n"
           "\t--rc-out-port PORT       set port num for rc out\n"
           "\t--base-port PORT         set port num for base port(default 5670) must be before -I option\n"
           "\t--defaults path          set path to defaults file\n"
        );
}

static const struct {
    const char *name;
    Aircraft *(*constructor)(const char *home_str, const char *frame_str);
} model_constructors[] = {
    { "quadplane",          QuadPlane::create },
    { "xplane",             XPlane::create },
    { "firefly",            QuadPlane::create },
    { "+",                  MultiCopter::create },
    { "quad",               MultiCopter::create },
    { "copter",             MultiCopter::create },
    { "x",                  MultiCopter::create },
    { "hexa",               MultiCopter::create },
    { "octa",               MultiCopter::create },
    { "tri",                MultiCopter::create },
    { "y6",                 MultiCopter::create },
    { "heli",               Helicopter::create },
    { "heli-dual",          Helicopter::create },
    { "heli-compound",      Helicopter::create },
    { "singlecopter",       SingleCopter::create },
    { "coaxcopter",         SingleCopter::create },
    { "rover",              SimRover::create },
    { "crrcsim",            CRRCSim::create },
    { "jsbsim",             JSBSim::create },
    { "flightaxis",         FlightAxis::create },
    { "gazebo",             Gazebo::create },
    { "last_letter",        last_letter::create },
    { "tracker",            Tracker::create },
    { "balloon",            Balloon::create },
    { "plane",              Plane::create },
    { "calibration",        Calibration::create },
    { "vectored",           Submarine::create },
};

void SITL_State::_set_signal_handlers(void) const
{
    struct sigaction sa_fpe = {};

    sigemptyset(&sa_fpe.sa_mask);
    sa_fpe.sa_handler = _sig_fpe;
    sigaction(SIGFPE, &sa_fpe, nullptr);

    struct sigaction sa_pipe = {};

    sigemptyset(&sa_pipe.sa_mask);
    sa_pipe.sa_handler = SIG_IGN; /* No-op SIGPIPE handler */
    sigaction(SIGPIPE, &sa_pipe, nullptr);
}

void SITL_State::_parse_command_line(int argc, char * const argv[])
{
    int opt;
    // default to CMAC
    const char *home_str = "-35.363261,149.165230,584,353";
    const char *model_str = nullptr;
    char *autotest_dir = nullptr;
    float speedup = 1.0f;

    if (asprintf(&autotest_dir, SKETCHBOOK "/Tools/autotest") <= 0) {
        AP_HAL::panic("out of memory");
    }

    _set_signal_handlers();

    setvbuf(stdout, (char *)0, _IONBF, 0);
    setvbuf(stderr, (char *)0, _IONBF, 0);

    _synthetic_clock_mode = false;
    const int BASE_PORT = 5760;
    const int RCOUT_PORT = 5502;
    const int RCIN_PORT = 5501;
    const int FG_VIEW_PORT = 5503;
    const int GAZEBO_IN_PORT = 9003;
    const int GAZEBO_OUT_PORT = 9002;
    const int IRLOCK_PORT = 9005;
    _base_port = BASE_PORT;
    _rcout_port = RCOUT_PORT;
    _rcin_port = RCIN_PORT;
    _fg_view_port = FG_VIEW_PORT;
    _fdm_address = "127.0.0.1";
    _client_address = nullptr;
    _use_fg_view = true;
    _gazebo_address = "127.0.0.1";
    _gazebo_port_in = GAZEBO_IN_PORT;
    _gazebo_port_out = GAZEBO_OUT_PORT;
    _irlock_port = IRLOCK_PORT;
    _instance = 0;

    enum long_options {
        CMDLINE_CLIENT=0,
        CMDLINE_GIMBAL,
        CMDLINE_AUTOTESTDIR,
        CMDLINE_UARTA,
        CMDLINE_UARTB,
        CMDLINE_UARTC,
        CMDLINE_UARTD,
        CMDLINE_UARTE,
        CMDLINE_UARTF,
        CMDLINE_RTSCTS,
        CMDLINE_FGVIEW,
        CMDLINE_GAZEBO_ADDRESS,
        CMDLINE_GAZEBO_PORT_IN,
        CMDLINE_GAZEBO_PORT_OUT,
        CMDLINE_BASE_PORT,
        CMDLINE_IRLOCK_PORT,
        CMDLINE_RCIN_PORT,
        CMDLINE_RCOUT_PORT,
        CMDLINE_DEFAULTS
    };

    const struct GetOptLong::option options[] = {
        {"help",            false,  0, 'h'},
        {"wipe",            false,  0, 'w'},
        {"unhide-groups",   false,  0, 'u'},
        {"speedup",         true,   0, 's'},
        {"rate",            true,   0, 'r'},
        {"console",         false,  0, 'C'},
        {"instance",        true,   0, 'I'},
        {"param",           true,   0, 'P'},
        {"synthetic-clock", false,  0, 'S'},
        {"home",            true,   0, 'O'},
        {"model",           true,   0, 'M'},
        {"uartA",           true,   0, CMDLINE_UARTA},
        {"uartB",           true,   0, CMDLINE_UARTB},
        {"uartC",           true,   0, CMDLINE_UARTC},
        {"uartD",           true,   0, CMDLINE_UARTD},
        {"uartE",           true,   0, CMDLINE_UARTE},
        {"client",          true,   0, CMDLINE_CLIENT},
        {"gimbal",          false,  0, CMDLINE_GIMBAL},
        {"autotest-dir",    true,   0, CMDLINE_AUTOTESTDIR},
        {"defaults",        true,   0, CMDLINE_DEFAULTS},
        {"rtscts",          false,  0, CMDLINE_RTSCTS},
        {"disable-fgview",  false,  0, CMDLINE_FGVIEW},
        {"gazebo-address",  true,   0, CMDLINE_GAZEBO_ADDRESS},
        {"gazebo-port-in",  true,   0, CMDLINE_GAZEBO_PORT_IN},
        {"gazebo-port-out", true,   0, CMDLINE_GAZEBO_PORT_OUT},
        {"base-port",       true,   0, CMDLINE_BASE_PORT},
        {"irlock-port",     true,   0, CMDLINE_IRLOCK_PORT},
        {"rc-in-port",      true,   0, CMDLINE_RCIN_PORT},
        {"rc-out-port",     true,   0, CMDLINE_RCOUT_PORT},
        {0, false, 0, 0}
    };

    GetOptLong gopt(argc, argv, "hwus:r:CI:P:SO:M:F:",
                    options);

    while ((opt = gopt.getoption()) != -1) {
        switch (opt) {
        case 'w':
            AP_Param::erase_all();
            unlink("dataflash.bin");
            break;
        case 'u':
            AP_Param::set_hide_disabled_groups(false);
            break;
        case 'r':
            _framerate = (unsigned)atoi(gopt.optarg);
            break;
        case 'C':
            HALSITL::UARTDriver::_console = true;
            break;
        case 'I': {
            _instance = atoi(gopt.optarg);
            if (_base_port == BASE_PORT) {
                _base_port += _instance * 10;
            }
            if (_rcout_port == RCOUT_PORT) {
                _rcout_port += _instance * 10;
            }
            if (_rcin_port == RCIN_PORT) {
                _rcin_port += _instance * 10;
            }
            if (_fg_view_port == FG_VIEW_PORT) {
                _fg_view_port += _instance * 10;
            }
            if (_gazebo_port_in == GAZEBO_IN_PORT) {
                _gazebo_port_in += _instance * 10;
            }
            if (_gazebo_port_out == GAZEBO_OUT_PORT) {
                _gazebo_port_out += _instance * 10;
            }
            if (_irlock_port == IRLOCK_PORT) {
                _irlock_port += _instance * 10;
            }
        }
        break;
        case 'P':
            _set_param_default(gopt.optarg);
            break;
        case 'S':
            _synthetic_clock_mode = true;
            break;
        case 'O':
            home_str = gopt.optarg;
            break;
        case 'M':
            model_str = gopt.optarg;
            break;
        case 's':
            speedup = strtof(gopt.optarg, nullptr);
            break;
        case 'F':
            _fdm_address = gopt.optarg;
            break;
        case CMDLINE_CLIENT:
            _client_address = gopt.optarg;
            break;
        case CMDLINE_GIMBAL:
            enable_gimbal = true;
            break;
        case CMDLINE_RTSCTS:
            _use_rtscts = true;
            break;
        case CMDLINE_AUTOTESTDIR:
            autotest_dir = strdup(gopt.optarg);
            break;
        case CMDLINE_DEFAULTS:
            defaults_path = strdup(gopt.optarg);
            break;

        case CMDLINE_UARTA:
        case CMDLINE_UARTB:
        case CMDLINE_UARTC:
        case CMDLINE_UARTD:
        case CMDLINE_UARTE:
        case CMDLINE_UARTF:
            _uart_path[opt - CMDLINE_UARTA] = gopt.optarg;
            break;
        case CMDLINE_FGVIEW:
            _use_fg_view = false;
            break;
        case CMDLINE_GAZEBO_ADDRESS:
            _gazebo_address = gopt.optarg;
            break;
        case CMDLINE_GAZEBO_PORT_IN:
            _gazebo_port_in = atoi(gopt.optarg);
            break;
        case CMDLINE_GAZEBO_PORT_OUT:
            _gazebo_port_out = atoi(gopt.optarg);
            break;
        case CMDLINE_IRLOCK_PORT:
            _irlock_port = atoi(gopt.optarg);
            break;
        case CMDLINE_RCIN_PORT:
            _rcin_port = atoi(gopt.optarg);
            break;
        case CMDLINE_RCOUT_PORT:
            _rcout_port = atoi(gopt.optarg);
            break;
        case CMDLINE_BASE_PORT:
            _base_port = atoi(gopt.optarg);
            break;
        default:
            _usage();
            exit(1);
        }
    }

    if (!model_str) {
        printf("You must specify a vehicle model\n");
        exit(1);
    }

    for (uint8_t i=0; i < ARRAY_SIZE(model_constructors); i++) {
        if (strncasecmp(model_constructors[i].name, model_str, strlen(model_constructors[i].name)) == 0) {
            sitl_model = model_constructors[i].constructor(home_str, model_str);
            if (strncasecmp("gazebo", model_str, strlen("gazebo")) == 0) {
              sitl_model->set_interface_ports(_gazebo_address, _gazebo_port_in, _gazebo_port_out);
            }
            sitl_model->set_speedup(speedup);
            sitl_model->set_instance(_instance);
            sitl_model->set_autotest_dir(autotest_dir);
            _synthetic_clock_mode = true;
            printf("Started model %s at %s at speed %.1f\n", model_str, home_str, speedup);
            break;
        }
    }
    if (sitl_model == nullptr) {
        printf("Vehicle model (%s) not found\n", model_str);
        exit(1);
    }

    fprintf(stdout, "Starting sketch '%s'\n", SKETCH);

    if (strcmp(SKETCH, "ArduCopter") == 0) {
        _vehicle = ArduCopter;
        if (_framerate == 0) {
            _framerate = 200;
        }
    } else if (strcmp(SKETCH, "APMrover2") == 0) {
        _vehicle = APMrover2;
        if (_framerate == 0) {
            _framerate = 50;
        }
        // set right default throttle for rover (allowing for reverse)
        pwm_input[2] = 1500;
    } else {
        _vehicle = ArduPlane;
        if (_framerate == 0) {
            _framerate = 50;
        }
    }

    _sitl_setup(home_str);
}

#endif
