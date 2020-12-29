#include "AP_Periph.h"

extern const AP_HAL::HAL &hal;

#ifndef HAL_PERIPH_LED_BRIGHT_DEFAULT
#define HAL_PERIPH_LED_BRIGHT_DEFAULT 100
#endif

#ifndef HAL_PERIPH_RANGEFINDER_BAUDRATE_DEFAULT
#define HAL_PERIPH_RANGEFINDER_BAUDRATE_DEFAULT 115200
#endif

#ifndef HAL_PERIPH_RANGEFINDER_PORT_DEFAULT
#define HAL_PERIPH_RANGEFINDER_PORT_DEFAULT 3
#endif

#ifndef HAL_PERIPH_GPS_PORT_DEFAULT
#define HAL_PERIPH_GPS_PORT_DEFAULT 3
#endif

#ifndef HAL_PERIPH_ADSB_BAUD_DEFAULT
#define HAL_PERIPH_ADSB_BAUD_DEFAULT 57600
#endif
#ifndef HAL_PERIPH_ADSB_PORT_DEFAULT
#define HAL_PERIPH_ADSB_PORT_DEFAULT 1
#endif

#ifndef AP_PERIPH_MSP_PORT_DEFAULT
#define AP_PERIPH_MSP_PORT_DEFAULT 1
#endif

/*
 *  AP_Periph parameter definitions
 *
 */

#define GSCALAR(v, name, def) { periph.g.v.vtype, name, Parameters::k_param_ ## v, &periph.g.v, {def_value : def} }
#define ASCALAR(v, name, def) { periph.aparm.v.vtype, name, Parameters::k_param_ ## v, (const void *)&periph.aparm.v, {def_value : def} }
#define GGROUP(v, name, class) { AP_PARAM_GROUP, name, Parameters::k_param_ ## v, &periph.g.v, {group_info : class::var_info} }
#define GOBJECT(v, name, class) { AP_PARAM_GROUP, name, Parameters::k_param_ ## v, (const void *)&periph.v, {group_info : class::var_info} }
#define GOBJECTN(v, pname, name, class) { AP_PARAM_GROUP, name, Parameters::k_param_ ## pname, (const void *)&periph.v, {group_info : class::var_info} }

const AP_Param::Info AP_Periph_FW::var_info[] = {
    // @Param: FORMAT_VERSION
    // @DisplayName: Eeprom format version number
    // @Description: This value is incremented when changes are made to the eeprom format
    // @User: Advanced
    GSCALAR(format_version,         "FORMAT_VERSION", 0),

    // can node number, 0 for dynamic node allocation
    GSCALAR(can_node,         "CAN_NODE", HAL_CAN_DEFAULT_NODE_ID),

    // can node baudrate
    GSCALAR(can_baudrate,     "CAN_BAUDRATE", 1000000),

#if !defined(HAL_NO_FLASH_SUPPORT) && !defined(HAL_NO_ROMFS_SUPPORT)
    // trigger bootloader flash
    GSCALAR(flash_bootloader,     "FLASH_BOOTLOADER", 0),
#endif

    GSCALAR(debug, "DEBUG", 0),

    GSCALAR(serial_number, "BRD_SERIAL_NUM", 0),

#ifdef HAL_PERIPH_ENABLE_BUZZER_WITHOUT_NOTIFY
    GSCALAR(buzz_volume,     "BUZZER_VOLUME", 100),
#endif

#ifdef HAL_PERIPH_ENABLE_GPS
    // GPS driver
    // @Group: GPS_
    // @Path: ../../libraries/AP_GPS/AP_GPS.cpp
    GOBJECT(gps, "GPS_", AP_GPS),

    // @Param: GPS_PORT
    // @DisplayName: GPS Serial Port
    // @Description: This is the serial port number where SERIALx_PROTOCOL will be set to GPS.
    // @Range: 0 10
    // @Increment: 1
    // @User: Advanced
    // @RebootRequired: True
    GSCALAR(gps_port, "GPS_PORT", HAL_PERIPH_GPS_PORT_DEFAULT),
#endif

#ifdef HAL_PERIPH_ENABLE_BATTERY
    // @Group: BATT
    // @Path: ../libraries/AP_BattMonitor/AP_BattMonitor.cpp
    GOBJECT(battery, "BATT", AP_BattMonitor),
#endif

#ifdef HAL_PERIPH_ENABLE_MAG
    // @Group: COMPASS_
    // @Path: ../../libraries/AP_Compass/AP_Compass.cpp
    GOBJECT(compass,         "COMPASS_",     Compass),
#endif

#ifdef HAL_PERIPH_ENABLE_BARO
    // Baro driver
    // @Group: BARO
    // @Path: ../../libraries/AP_Baro/AP_Baro.cpp
    GOBJECT(baro, "BARO", AP_Baro),
    GSCALAR(baro_enable, "BARO_ENABLE", 1),
#endif

#ifdef AP_PERIPH_HAVE_LED_WITHOUT_NOTIFY
    GSCALAR(led_brightness, "LED_BRIGHTNESS", HAL_PERIPH_LED_BRIGHT_DEFAULT),
#endif

#ifdef HAL_PERIPH_ENABLE_AIRSPEED
    // Airspeed driver
    // @Group: ARSP
    // @Path: ../../libraries/AP_Airspeed/AP_Airspeed.cpp
    GOBJECT(airspeed, "ARSP", AP_Airspeed),
#endif

#ifdef HAL_PERIPH_ENABLE_RANGEFINDER
    GSCALAR(rangefinder_baud, "RNGFND_BAUDRATE", HAL_PERIPH_RANGEFINDER_BAUDRATE_DEFAULT),
    GSCALAR(rangefinder_port, "RNGFND_PORT", HAL_PERIPH_RANGEFINDER_PORT_DEFAULT),

    // Rangefinder driver
    // @Group: RNGFND
    // @Path: ../../libraries/AP_RangeFinder/Rangefinder.cpp
    GOBJECT(rangefinder, "RNGFND", RangeFinder),
#endif

#ifdef HAL_PERIPH_ENABLE_ADSB
    GSCALAR(adsb_baudrate, "ADSB_BAUDRATE", HAL_PERIPH_ADSB_BAUD_DEFAULT),
    GSCALAR(adsb_port, "ADSB_PORT", HAL_PERIPH_ADSB_PORT_DEFAULT),
#endif

#ifdef HAL_PERIPH_ENABLE_PWM_HARDPOINT
    GSCALAR(hardpoint_id, "HARDPOINT_ID", HAL_PWM_HARDPOINT_ID_DEFAULT),
    GSCALAR(hardpoint_rate, "HARDPOINT_RATE", 100),
#endif

#ifdef HAL_PERIPH_ENABLE_HWESC
    GSCALAR(esc_number, "ESC_NUMBER", 0),
#endif

#ifdef HAL_PERIPH_ENABLE_RC_OUT
    // Servo driver
    // @Group: OUT
    // @Path: ../libraries/SRV_Channel/SRV_Channels.cpp
    GOBJECT(servo_channels, "OUT",     SRV_Channels),
#endif

#ifdef HAL_PERIPH_ENABLE_MSP
    GSCALAR(msp_port, "MSP_PORT", AP_PERIPH_MSP_PORT_DEFAULT),
#endif
    
#ifdef HAL_PERIPH_ENABLE_NOTIFY
    // @Group: NTF_
    // @Path: ../libraries/AP_Notify/AP_Notify.cpp
    GOBJECT(notify, "NTF_",  AP_Notify),
#endif

    AP_VAREND
};


void AP_Periph_FW::load_parameters(void)
{
    AP_Param::setup_sketch_defaults();

    if (!AP_Param::check_var_info()) {
        hal.console->printf("Bad parameter table\n");
        AP_HAL::panic("Bad parameter table");
    }
    if (!g.format_version.load() ||
        g.format_version != Parameters::k_format_version) {
        // erase all parameters
        StorageManager::erase();
        AP_Param::erase_all();

        // save the current format version
        g.format_version.set_and_save(Parameters::k_format_version);
    }

    // Load all auto-loaded EEPROM variables
    AP_Param::load_all();
}
