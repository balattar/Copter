/*****************************************************************************
The init_ardupilot function processes everything we need for an in - air restart
    We will determine later if we are actually on the ground and process a
    ground start in that case.

*****************************************************************************/

#include "Rover.h"
#include "version.h"

#if CLI_ENABLED == ENABLED

// This is the help function
int8_t Rover::main_menu_help(uint8_t argc, const Menu::arg *argv)
{
    cliSerial->printf("Commands:\n"
                      "  logs        log readback/setup mode\n"
                      "  setup       setup mode\n"
                      "  test        test mode\n"
                      "\n"
                      "Move the slide switch and reset to FLY.\n"
                      "\n");
    return(0);
}

// Command/function table for the top-level menu.

static const struct Menu::command main_menu_commands[] = {
//   command        function called
//   =======        ===============
    {"logs",        MENU_FUNC(process_logs)},
    {"setup",       MENU_FUNC(setup_mode)},
    {"test",        MENU_FUNC(test_mode)},
    {"reboot",      MENU_FUNC(reboot_board)},
    {"help",        MENU_FUNC(main_menu_help)}
};

// Create the top-level menu object.
MENU(main_menu, THISFIRMWARE, main_menu_commands);

int8_t Rover::reboot_board(uint8_t argc, const Menu::arg *argv)
{
    hal.scheduler->reboot(false);
    return 0;
}

// the user wants the CLI. It never exits
void Rover::run_cli(AP_HAL::UARTDriver *port)
{
    // disable the failsafe code in the CLI
    hal.scheduler->register_timer_failsafe(nullptr, 1);

    // disable the mavlink delay callback
    hal.scheduler->register_delay_callback(nullptr, 5);

    cliSerial = port;
    Menu::set_port(port);
    port->set_blocking_writes(true);

    while (1) {
        main_menu.run();
    }
}

#endif  // CLI_ENABLED

static void mavlink_delay_cb_static()
{
    rover.mavlink_delay_cb();
}

static void failsafe_check_static()
{
    rover.failsafe_check();
}

void Rover::init_ardupilot()
{
    // initialise console serial port
    serial_manager.init_console();

    cliSerial->printf("\n\nInit " FIRMWARE_STRING
                      "\n\nFree RAM: %u\n",
                      hal.util->available_memory());

    //
    // Check the EEPROM format version before loading any parameters from EEPROM.
    //

    load_parameters();

    // initialise stats module
    g2.stats.init();

    gcs().set_dataflash(&DataFlash);

    mavlink_system.sysid = g.sysid_this_mav;

    // initialise serial ports
    serial_manager.init();

    // setup first port early to allow BoardConfig to report errors
    gcs().chan(0).setup_uart(serial_manager, AP_SerialManager::SerialProtocol_MAVLink, 0);

    // Register mavlink_delay_cb, which will run anytime you have
    // more than 5ms remaining in your call to hal.scheduler->delay
    hal.scheduler->register_delay_callback(mavlink_delay_cb_static, 5);

    // specify callback function for CLI menu system
#if CLI_ENABLED == ENABLED
    if (gcs().cli_enabled()) {
        gcs().set_run_cli_func(FUNCTOR_BIND_MEMBER(&Rover::run_cli, void, AP_HAL::UARTDriver *));
    }
#endif

    BoardConfig.init();
#if HAL_WITH_UAVCAN
    BoardConfig_CAN.init();
#endif

    // initialise notify system
    notify.init(false);
    AP_Notify::flags.failsafe_battery = false;
    notify_mode(control_mode);

    ServoRelayEvents.set_channel_mask(0xFFF0);

    battery.init();

    rssi.init();

    // keep a record of how many resets have happened. This can be
    // used to detect in-flight resets
    g.num_resets.set_and_save(g.num_resets+1);

    // init baro before we start the GCS, so that the CLI baro test works
    barometer.init();

    // we start by assuming USB connected, as we initialed the serial
    // port with SERIAL0_BAUD. check_usb_mux() fixes this if need be.
    usb_connected = true;
    check_usb_mux();

    // setup telem slots with serial ports
    gcs().setup_uarts(serial_manager);

    // setup frsky telemetry
#if FRSKY_TELEM_ENABLED == ENABLED
    frsky_telemetry.init(serial_manager, FIRMWARE_STRING, MAV_TYPE_GROUND_ROVER);
#endif

#if LOGGING_ENABLED == ENABLED
    log_init();
#endif

    // initialise compass
    init_compass();

    // initialise sonar
    init_sonar();

    // init beacons used for non-gps position estimation
    init_beacon();

    // init visual odometry
    init_visual_odom();

    // and baro for EKF
    init_barometer(true);

    // Do GPS init
    gps.set_log_gps_bit(MASK_LOG_GPS);
    gps.init(serial_manager);

    rc_override_active = hal.rcin->set_overrides(rc_override, 8);

    ins.set_log_raw_bit(MASK_LOG_IMU_RAW);

    set_control_channels();
    init_rc_in();        // sets up rc channels from radio
    init_rc_out();        // sets up the timer libs

    relay.init();

#if MOUNT == ENABLED
    // initialise camera mount
    camera_mount.init(&DataFlash, serial_manager);
#endif

    /*
      setup the 'main loop is dead' check. Note that this relies on
      the RC library being initialised.
     */
    hal.scheduler->register_timer_failsafe(failsafe_check_static, 1000);

    // give AHRS the range beacon sensor
    ahrs.set_beacon(&g2.beacon);


#if CLI_ENABLED == ENABLED
    gcs().handle_interactive_setup();
#endif

    init_capabilities();

    startup_ground();

    set_mode((enum mode)g.initial_mode.get());

    // set the correct flight mode
    // ---------------------------
    reset_control_switch();

    // disable safety if requested
    BoardConfig.init_safety();

    // flag that initialisation has completed
    initialised = true;
}

//*********************************************************************************
// This function does all the calibrations, etc. that we need during a ground start
//*********************************************************************************
void Rover::startup_ground(void)
{
    set_mode(INITIALISING);

    gcs_send_text(MAV_SEVERITY_INFO, "<startup_ground> Ground start");

    #if(GROUND_START_DELAY > 0)
        gcs_send_text(MAV_SEVERITY_NOTICE, "<startup_ground> With delay");
        delay(GROUND_START_DELAY * 1000);
    #endif

    // IMU ground start
    //------------------------
    //

    startup_INS_ground();

    // read the radio to set trims
    // ---------------------------
    trim_radio();

    // initialise mission library
    mission.init();

    // initialise DataFlash library
    DataFlash.set_mission(&mission);
    DataFlash.setVehicle_Startup_Log_Writer(
        FUNCTOR_BIND(&rover, &Rover::Log_Write_Vehicle_Startup_Messages, void)
        );

    // we don't want writes to the serial port to cause us to pause
    // so set serial ports non-blocking once we are ready to drive
    serial_manager.set_blocking_writes_all(false);

    gcs_send_text(MAV_SEVERITY_INFO, "Ready to drive");
}

/*
  set the in_reverse flag
  reset the throttle integrator if this changes in_reverse
 */
void Rover::set_reverse(bool reverse)
{
    if (in_reverse == reverse) {
        return;
    }
    g.pidSpeedThrottle.reset_I();
    steerController.reset_I();
    nav_controller->set_reverse(reverse);
    steerController.set_reverse(reverse);
    in_reverse = reverse;
}

void Rover::set_mode(enum mode mode)
{
    if (control_mode == mode) {
        // don't switch modes if we are already in the correct mode.
        return;
    }

    // If we are changing out of AUTO mode reset the loiter timer and stop current mission
    if (control_mode == AUTO) {
        loiter_start_time = 0;
        if (mission.state() == AP_Mission::MISSION_RUNNING) {
            mission.stop();
        }
    }

    control_mode = mode;
    throttle = 500;
    if (!in_auto_reverse) {
        set_reverse(false);
    }
    g.pidSpeedThrottle.reset_I();

#if FRSKY_TELEM_ENABLED == ENABLED
    frsky_telemetry.update_control_mode(control_mode);
#endif

    if (control_mode != AUTO) {
        auto_triggered = false;
    }

    switch (control_mode) {
    case MANUAL:
    case HOLD:
    case LEARNING:
    case STEERING:
        auto_throttle_mode = false;
        break;

    case AUTO:
        auto_throttle_mode = true;
        rtl_complete = false;
        restart_nav();
        break;

    case RTL:
        auto_throttle_mode = true;
        do_RTL();
        break;

    case GUIDED:
        auto_throttle_mode = true;
        /*
           when entering guided mode we set the target as the current
           location. This matches the behaviour of the copter code.
           */
        set_guided_WP(current_loc);
        break;

    default:
        auto_throttle_mode = true;
        do_RTL();
        break;
    }

    if (should_log(MASK_LOG_MODE)) {
        DataFlash.Log_Write_Mode(control_mode);
    }

    notify_mode(control_mode);
}

/*
  set_mode() wrapper for MAVLink SET_MODE
 */
bool Rover::mavlink_set_mode(uint8_t mode)
{
    switch (mode) {
    case MANUAL:
    case HOLD:
    case LEARNING:
    case STEERING:
    case GUIDED:
    case AUTO:
    case RTL:
        set_mode((enum mode)mode);
        return true;
    }
    return false;
}

void Rover::startup_INS_ground(void)
{
    gcs_send_text(MAV_SEVERITY_INFO, "Warming up ADC");
    mavlink_delay(500);

    // Makes the servos wiggle twice - about to begin INS calibration - HOLD LEVEL AND STILL!!
    // -----------------------
    gcs_send_text(MAV_SEVERITY_INFO, "Beginning INS calibration. Do not move vehicle");
    mavlink_delay(1000);

    ahrs.init();
    // say to EKF that rover only move by goind forward
    ahrs.set_fly_forward(true);
    ahrs.set_vehicle_class(AHRS_VEHICLE_GROUND);

    ins.init(scheduler.get_loop_rate_hz());
    ahrs.reset();
}

// updates the notify state
// should be called at 50hz
void Rover::update_notify()
{
    notify.update();
}

void Rover::resetPerfData(void) {
    mainLoop_count = 0;
    G_Dt_max = 0;
    perf_mon_timer = millis();
}


void Rover::check_usb_mux(void)
{
    bool usb_check = hal.gpio->usb_connected();
    if (usb_check == usb_connected) {
        return;
    }

    // the user has switched to/from the telemetry port
    usb_connected = usb_check;
}


void Rover::print_mode(AP_HAL::BetterStream *port, uint8_t mode)
{
    switch (mode) {
    case MANUAL:
        port->printf("Manual");
        break;
    case HOLD:
        port->printf("HOLD");
        break;
    case LEARNING:
        port->printf("Learning");
        break;
    case STEERING:
        port->printf("Steering");
        break;
    case AUTO:
        port->printf("AUTO");
        break;
    case RTL:
        port->printf("RTL");
        break;
    default:
        port->printf("Mode(%u)", static_cast<uint32_t>(mode));
        break;
    }
}

// update notify with mode change
void Rover::notify_mode(enum mode new_mode)
{
    notify.flags.flight_mode = new_mode;

    switch (new_mode) {
    case MANUAL:
        notify.set_flight_mode_str("MANU");
        break;
    case LEARNING:
        notify.set_flight_mode_str("LERN");
        break;
    case STEERING:
        notify.set_flight_mode_str("STER");
        break;
    case HOLD:
        notify.set_flight_mode_str("HOLD");
        break;
    case AUTO:
        notify.set_flight_mode_str("AUTO");
        break;
    case RTL:
        notify.set_flight_mode_str("RTL");
        break;
    case GUIDED:
        notify.set_flight_mode_str("GUID");
        break;
    case INITIALISING:
        notify.set_flight_mode_str("INIT");
        break;
    default:
        notify.set_flight_mode_str("----");
        break;
    }
}

/*
  check a digitial pin for high,low (1/0)
 */
uint8_t Rover::check_digital_pin(uint8_t pin)
{
    const int8_t dpin = hal.gpio->analogPinToDigitalPin(pin);
    if (dpin == -1) {
        return 0;
    }
    // ensure we are in input mode
    hal.gpio->pinMode(dpin, HAL_GPIO_INPUT);

    // enable pullup
    hal.gpio->write(dpin, 1);

    return hal.gpio->read(dpin);
}

/*
  should we log a message type now?
 */
bool Rover::should_log(uint32_t mask)
{
    if (!DataFlash.should_log(mask)) {
        return false;
    }
    start_logging();
    return true;
}

/*
  update AHRS soft arm state and log as needed
 */
void Rover::change_arm_state(void)
{
    Log_Arm_Disarm();
    update_soft_armed();
}

/*
  arm motors
 */
bool Rover::arm_motors(AP_Arming::ArmingMethod method)
{
    if (!arming.arm(method)) {
        return false;
    }

    change_arm_state();
    return true;
}

/*
  disarm motors
 */
bool Rover::disarm_motors(void)
{
    if (!arming.disarm()) {
        return false;
    }
    if (control_mode != AUTO) {
        // reset the mission on disarm if we are not in auto
        mission.reset();
    }

    // only log if disarming was successful
    change_arm_state();

    return true;
}
