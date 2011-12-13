# fly ArduPlane in SIL

import util, pexpect, sys, time, math, shutil, os
from common import *
import mavutil, random

# get location of scripts
testdir=os.path.dirname(os.path.realpath(__file__))


HOME_LOCATION='-35.362938,149.165085,584,270'

homeloc = None

def takeoff(mavproxy, mav):
    '''takeoff get to 30m altitude'''
    mavproxy.send('switch 4\n')
    wait_mode(mav, 'FBWA')

    # some rudder to counteract the prop torque
    mavproxy.send('rc 4 1600\n')

    # get it moving a bit first to avoid bad JSBSim ground physics
    mavproxy.send('rc 3 1040\n')
    mav.recv_match(condition='VFR_HUD.groundspeed>3', blocking=True)

    # a bit faster
    mavproxy.send('rc 3 1600\n')
    mav.recv_match(condition='VFR_HUD.groundspeed>10', blocking=True)

    # hit the gas harder now, and give it some elevator
    mavproxy.send('rc 4 1500\n')
    mavproxy.send('rc 2 1200\n')
    mavproxy.send('rc 3 1800\n')

    # gain a bit of altitude
    wait_altitude(mav, homeloc.alt+30, homeloc.alt+60, timeout=30)

    # level off
    mavproxy.send('rc 2 1500\n')

    print("TAKEOFF COMPLETE")
    return True

def fly_left_circuit(mavproxy, mav):
    '''fly a left circuit, 200m on a side'''
    mavproxy.send('switch 4\n')
    wait_mode(mav, 'FBWA')
    mavproxy.send('rc 3 2000\n')
    wait_level_flight(mavproxy, mav)

    print("Flying left circuit")
    # do 4 turns
    for i in range(0,4):
        # hard left
        print("Starting turn %u" % i)
        mavproxy.send('rc 1 1000\n')
        wait_heading(mav, 270 - (90*i))
        mavproxy.send('rc 1 1500\n')
        print("Starting leg %u" % i)
        wait_distance(mav, 100)
    print("Circuit complete")
    return True

def fly_RTL(mavproxy, mav):
    '''fly to home'''
    print("Flying home in RTL")
    mavproxy.send('switch 2\n')
    wait_mode(mav, 'RTL')
    wait_location(mav, homeloc, accuracy=80,
                  target_altitude=100, height_accuracy=10)
    print("RTL Complete")
    return True

def fly_LOITER(mavproxy, mav):
    '''loiter where we are'''
    print("Testing LOITER")
    mavproxy.send('switch 3\n')
    wait_mode(mav, 'LOITER')
    while True:
        wait_heading(mav, 0)
        wait_heading(mav, 180)
    return True


def wait_level_flight(mavproxy, mav, accuracy=5, timeout=30):
    '''wait for level flight'''
    tstart = time.time()
    while time.time() < tstart + timeout:
        m = mav.recv_match(type='ATTITUDE', blocking=True)
        roll = math.degrees(m.roll)
        pitch = math.degrees(m.pitch)
        print("Roll=%.1f Pitch=%.1f" % (roll, pitch))
        if math.fabs(roll) <= accuracy and math.fabs(pitch) <= accuracy:
            return True
    print("Failed to attain level flight")
    return False


def change_altitude(mavproxy, mav, altitude, accuracy=10):
    '''get to a given altitude'''
    mavproxy.send('switch 4\n')
    wait_mode(mav, 'FBWA')
    alt_error = mav.messages['VFR_HUD'].alt - altitude
    if alt_error > 0:
        mavproxy.send('rc 2 2000\n')
    else:
        mavproxy.send('rc 2 1000\n')
    wait_altitude(mav, altitude-accuracy/2, altitude+accuracy/2)
    mavproxy.send('rc 2 1500\n')
    print("Reached target altitude at %u" % mav.messages['VFR_HUD'].alt)
    return wait_level_flight(mavproxy, mav)


def axial_left_roll(mavproxy, mav, count=1):
    '''fly a left axial roll'''
    # full throttle!
    mavproxy.send('rc 3 2000\n')
    change_altitude(mavproxy, mav, homeloc.alt+200)

    # fly the roll in manual
    mavproxy.send('switch 6\n')
    wait_mode(mav, 'MANUAL')

    while count > 0:
        print("Starting roll")
        mavproxy.send('rc 1 1000\n')
        wait_roll(mav, -150, accuracy=20)
        wait_roll(mav, 150, accuracy=20)
        wait_roll(mav, 0, accuracy=20)
        count -= 1

    # back to FBWA
    mavproxy.send('rc 1 1500\n')
    mavproxy.send('switch 4\n')
    wait_mode(mav, 'FBWA')
    mavproxy.send('rc 3 1700\n')
    return wait_level_flight(mavproxy, mav)


def inside_loop(mavproxy, mav, count=1):
    '''fly a inside loop'''
    # full throttle!
    mavproxy.send('rc 3 2000\n')
    change_altitude(mavproxy, mav, homeloc.alt+200)

    # fly the loop in manual
    mavproxy.send('switch 6\n')
    wait_mode(mav, 'MANUAL')

    while count > 0:
        print("Starting loop")
        mavproxy.send('rc 2 1000\n')
        wait_pitch(mav, 80, accuracy=20)
        wait_pitch(mav, 0, accuracy=20)
        count -= 1

    # back to FBWA
    mavproxy.send('rc 2 1500\n')
    mavproxy.send('switch 4\n')
    wait_mode(mav, 'FBWA')
    mavproxy.send('rc 3 1700\n')
    return wait_level_flight(mavproxy, mav)



def setup_rc(mavproxy):
    '''setup RC override control'''
    for chan in [1,2,4,5,6,7]:
        mavproxy.send('rc %u 1500\n' % chan)
    mavproxy.send('rc 3 1000\n')
    mavproxy.send('rc 8 1800\n')


def fly_mission(mavproxy, mav, filename, height_accuracy=-1, target_altitude=None):
    '''fly a mission from a file'''
    global homeloc
    mavproxy.send('wp load %s\n' % filename)
    mavproxy.expect('flight plan received')
    mavproxy.send('wp list\n')
    mavproxy.expect('Requesting [0-9]+ waypoints')
    mavproxy.send('switch 1\n') # auto mode
    wait_mode(mav, 'AUTO')
    if not wait_distance(mav, 30, timeout=120):
        return False
    if not wait_location(mav, homeloc, accuracy=50, timeout=600, target_altitude=target_altitude, height_accuracy=height_accuracy):
        return False
    print("Mission OK")
    return True


def fly_ArduPlane(viewerip=None):
    '''fly ArduPlane in SIL

    you can pass viewerip as an IP address to optionally send fg and
    mavproxy packets too for local viewing of the flight in real time
    '''
    global homeloc

    options = '--sitl=127.0.0.1:5501 --out=127.0.0.1:19550 --streamrate=5'
    if viewerip:
        options += " --out=%s:14550" % viewerip

    sil = util.start_SIL('ArduPlane', wipe=True)
    mavproxy = util.start_MAVProxy_SIL('ArduPlane', options=options)
    mavproxy.expect('Received [0-9]+ parameters')

    # setup test parameters
    mavproxy.send('param set SYSID_THISMAV %u\n' % random.randint(100, 200))
    mavproxy.send("param load %s/ArduPlane.parm\n" % testdir)
    mavproxy.expect('Loaded [0-9]+ parameters')

    # restart with new parms
    util.pexpect_close(mavproxy)
    util.pexpect_close(sil)

    cmd = util.reltopdir("Tools/autotest/jsbsim/runsim.py")
    cmd += " --home=%s --wind=5,180,0.2" % (
        HOME_LOCATION, util.reltopdir("Tools/autotest/jsbsim"))
    if viewerip:
        cmd += " --fgout=%s:5503" % viewerip

    runsim = pexpect.spawn(cmd, logfile=sys.stdout, timeout=10)
    runsim.delaybeforesend = 0
    util.pexpect_autoclose(runsim)
    runsim.expect('Simulator ready to fly')

    sil = util.start_SIL('ArduPlane')
    mavproxy = util.start_MAVProxy_SIL('ArduPlane', options=options)
    mavproxy.expect('Logging to (\S+)')
    logfile = mavproxy.match.group(1)
    print("LOGFILE %s" % logfile)

    buildlog = util.reltopdir("../buildlogs/ArduPlane-test.mavlog")
    print("buildlog=%s" % buildlog)
    if os.path.exists(buildlog):
        os.unlink(buildlog)
    os.link(logfile, buildlog)

    mavproxy.expect('Received [0-9]+ parameters')

    util.expect_setup_callback(mavproxy, expect_callback)

    expect_list_clear()
    expect_list_extend([runsim, sil, mavproxy])

    print("Started simulator")

    # get a mavlink connection going
    try:
        mav = mavutil.mavlink_connection('127.0.0.1:19550', robust_parsing=True)
    except Exception, msg:
        print("Failed to start mavlink connection on 127.0.0.1:19550" % msg)
        raise
    mav.message_hooks.append(message_hook)
    mav.idle_hooks.append(idle_hook)

    failed = False
    e = 'None'
    try:
        mav.wait_heartbeat()
        setup_rc(mavproxy)
        mavproxy.expect('APM: init home')
        mav.recv_match(type='GPS_RAW', condition='MAV.flightmode!="INITIALISING" and GPS_RAW.fix_type==2 and GPS_RAW.lat != 0 and GPS_RAW.alt != 0 and VFR_HUD.alt > 10',
                       blocking=True)
        homeloc = current_location(mav)
        print("Home location: %s" % homeloc)
        if not takeoff(mavproxy, mav):
            print("Failed takeoff")
            failed = True
        if not fly_left_circuit(mavproxy, mav):
            print("Failed left circuit")
            failed = True
        if not axial_left_roll(mavproxy, mav, 1):
            print("Failed left roll")
            failed = True
        if not inside_loop(mavproxy, mav):
            print("Failed inside loop")
            failed = True
        if not fly_RTL(mavproxy, mav):
            print("Failed RTL")
            failed = True
        if not fly_mission(mavproxy, mav, os.path.join(testdir, "ap1.txt"), height_accuracy = 10,
                           target_altitude=homeloc.alt+100):
            print("Failed mission")
            failed = True
    except pexpect.TIMEOUT, e:
        failed = True

    mav.close()
    util.pexpect_close(mavproxy)
    util.pexpect_close(sil)
    util.pexpect_close(runsim)

    if os.path.exists('ArduPlane-valgrind.log'):
        os.chmod('ArduPlane-valgrind.log', 0644)
        shutil.copy("ArduPlane-valgrind.log", util.reltopdir("../buildlogs/ArduPlane-valgrind.log"))

    if failed:
        print("FAILED: %s" % e)
        return False
    return True
