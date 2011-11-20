// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
//
//  DO NOT EDIT this file to adjust your configuration.  Create your own
//  APM_Config.h and use APM_Config.h.example as a reference.
//
// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
///
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Default and automatic configuration details.
//
// Notes for maintainers:
//
// - Try to keep this file organised in the same order as APM_Config.h.example
//

#include "defines.h"

///
/// DO NOT EDIT THIS INCLUDE - if you want to make a local change, make that
/// change in your local copy of APM_Config.h.
///
#include "APM_Config.h"  // <== THIS INCLUDE, DO NOT EDIT IT. EVER.

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// HARDWARE CONFIGURATION AND CONNECTIONS
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// APM HARDWARE
//

#ifndef CONFIG_APM_HARDWARE
# define CONFIG_APM_HARDWARE APM_HARDWARE_APM1
#endif

//////////////////////////////////////////////////////////////////////////////
// PURPLE HARDWARE DEFAULTS
//

#if CONFIG_APM_HARDWARE == APM_HARDWARE_PURPLE
# define CONFIG_IMU_TYPE   CONFIG_IMU_MPU6000
# define CONFIG_PUSHBUTTON DISABLED
# define CONFIG_RELAY      DISABLED
# define MAG_ORIENTATION   AP_COMPASS_APM2_SHIELD
# define CONFIG_SONAR_SOURCE SONAR_SOURCE_ANALOG_PIN
#endif


//////////////////////////////////////////////////////////////////////////////
// FRAME_CONFIG
//
#ifndef FRAME_CONFIG
# define FRAME_CONFIG		QUAD_FRAME
#endif
#ifndef FRAME_ORIENTATION
# define FRAME_ORIENTATION		PLUS_FRAME
#endif

//////////////////////////////////////////////////////////////////////////////
// IMU Selection
//
#ifndef CONFIG_IMU_TYPE
# define CONFIG_IMU_TYPE CONFIG_IMU_OILPAN
#endif

#if CONFIG_IMU_TYPE == CONFIG_IMU_MPU6000
# ifndef CONFIG_MPU6000_CHIP_SELECT_PIN
#  define CONFIG_MPU6000_CHIP_SELECT_PIN 53
# endif
#endif


//////////////////////////////////////////////////////////////////////////////
// ADC Enable - used to eliminate for systems which don't have ADC.
//
#ifndef CONFIG_ADC
# if CONFIG_IMU_TYPE == CONFIG_IMU_OILPAN
#   define CONFIG_ADC ENABLED
# else
#   define CONFIG_ADC DISABLED
# endif
#endif

//////////////////////////////////////////////////////////////////////////////
// PWM control
//
#ifndef INSTANT_PWM
# define INSTANT_PWM	DISABLED
#endif

// LED and IO Pins
//
#if CONFIG_APM_HARDWARE == APM_HARDWARE_APM1
# define A_LED_PIN        37
# define B_LED_PIN        36
# define C_LED_PIN        35
# define LED_ON           HIGH
# define LED_OFF          LOW
# define SLIDE_SWITCH_PIN 40
# define PUSHBUTTON_PIN   41
# define USB_MUX_PIN      -1
#elif CONFIG_APM_HARDWARE == APM_HARDWARE_PURPLE
# define A_LED_PIN        27
# define B_LED_PIN        26
# define C_LED_PIN        25
# define LED_ON           LOW
# define LED_OFF          HIGH
# define SLIDE_SWITCH_PIN (-1)
# define PUSHBUTTON_PIN   (-1)
# define CLI_SLIDER_ENABLED DISABLED
# define USB_MUX_PIN 23
#endif

//////////////////////////////////////////////////////////////////////////////
// Pushbutton & Relay
//

#ifndef CONFIG_PUSHBUTTON
# define CONFIG_PUSHBUTTON ENABLED
#endif

#ifndef CONFIG_RELAY
# define CONFIG_RELAY ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// Sonar
//

#ifndef SONAR_PORT
# define SONAR_PORT		AP_RANGEFINDER_PITOT_TUBE
#endif

#ifndef CONFIG_SONAR_SOURCE
# define CONFIG_SONAR_SOURCE SONAR_SOURCE_ADC
#endif

#if CONFIG_SONAR_SOURCE == SONAR_SOURCE_ADC && CONFIG_ADC == DISABLED
# warning Cannot use ADC for CONFIG_SONAR_SOURCE, becaude CONFIG_ADC is DISABLED
# warning Defaulting CONFIG_SONAR_SOURCE to ANALOG_PIN
# undef CONFIG_SONAR_SOURCE
# define CONFIG_SONAR_SOURCE SONAR_SOURCE_ANALOG_PIN
#endif

#if CONFIG_SONAR_SOURCE == SONAR_SOURCE_ADC
# ifndef CONFIG_SONAR_SOURCE_ADC_CHANNEL
#  define CONFIG_SONAR_SOURCE_ADC_CHANNEL 7
# endif
#elif CONFIG_SONAR_SOURCE == SONAR_SOURCE_ANALOG_PIN
# ifndef CONFIG_SONAR_SOURCE_ANALOG_PIN
#  define CONFIG_SONAR_SOURCE_ANALOG_PIN AN4
# endif
#else
# warning Invalid value for CONFIG_SONAR_SOURCE, disabling sonar
# undef SONAR_ENABLED
# define SONAR_ENABLED DISABLED
#endif

#ifndef SONAR_TYPE
# define SONAR_TYPE		MAX_SONAR_XL
#endif

// It seems that MAX_SONAR_XL depends on an ADC. For systems without an
// ADC, we need to disable the sonar
#if SONAR_TYPE == MAX_SONAR_XL
# if CONFIG_ADC == DISABLED
#   if defined(CONFIG_SONAR)
#      warning "MAX_SONAR_XL requires a valid ADC. This system does not have an ADC enabled."
#      undef CONFIG_SONAR
#   endif
#   define CONFIG_SONAR DISABLED
#  endif
#endif

#ifndef CONFIG_SONAR
# define CONFIG_SONAR ENABLED
#endif


//////////////////////////////////////////////////////////////////////////////
// Acrobatics
//

#ifndef CH7_OPTION
# define CH7_OPTION		CH7_SAVE_WP
#endif


//////////////////////////////////////////////////////////////////////////////
// AIRSPEED_SENSOR
// AIRSPEED_RATIO
//
#ifndef AIRSPEED_SENSOR
# define AIRSPEED_SENSOR		DISABLED
#endif
#ifndef AIRSPEED_RATIO
# define AIRSPEED_RATIO			1.9936		// Note - this varies from the value in ArduPilot due to the difference in ADC resolution
#endif


//////////////////////////////////////////////////////////////////////////////
// HIL_MODE                                 OPTIONAL

#ifndef HIL_MODE
#define HIL_MODE	HIL_MODE_DISABLED
#endif

#if HIL_MODE != HIL_MODE_DISABLED	// we are in HIL mode

 # undef GPS_PROTOCOL
 # define GPS_PROTOCOL GPS_PROTOCOL_NONE

#endif


//////////////////////////////////////////////////////////////////////////////
// GPS_PROTOCOL
//
#ifndef GPS_PROTOCOL
# define GPS_PROTOCOL 		GPS_PROTOCOL_AUTO
#endif


#ifndef MAV_SYSTEM_ID
# define MAV_SYSTEM_ID		1
#endif


//////////////////////////////////////////////////////////////////////////////
// Serial port speeds.
//
#ifndef SERIAL0_BAUD
# define SERIAL0_BAUD			115200
#endif
#ifndef SERIAL3_BAUD
# define SERIAL3_BAUD			 57600
#endif


//////////////////////////////////////////////////////////////////////////////
// Battery monitoring
//
#ifndef BATTERY_EVENT
# define BATTERY_EVENT			DISABLED
#endif
#ifndef LOW_VOLTAGE
# define LOW_VOLTAGE			9.6
#endif
#ifndef VOLT_DIV_RATIO
# define VOLT_DIV_RATIO			3.56
#endif

#ifndef CURR_AMP_PER_VOLT
# define CURR_AMP_PER_VOLT		27.32
#endif
#ifndef CURR_AMPS_OFFSET
# define CURR_AMPS_OFFSET		0.0
#endif
#ifndef HIGH_DISCHARGE
# define HIGH_DISCHARGE			1760
#endif


#ifndef PIEZO
# define PIEZO				ENABLED				//Enables Piezo Code and beeps once on Startup to verify operation
#endif
#ifndef PIEZO_LOW_VOLTAGE
# define PIEZO_LOW_VOLTAGE	ENABLED				//Enables Tone on reaching low battery or current alert
#endif
#ifndef PIEZO_ARMING
# define PIEZO_ARMING		ENABLED				//Two tones on ARM, 1 Tone on disarm
#endif


//////////////////////////////////////////////////////////////////////////////
// INPUT_VOLTAGE
//
#ifndef INPUT_VOLTAGE
# define INPUT_VOLTAGE			5.0
#endif


//////////////////////////////////////////////////////////////////////////////
//  MAGNETOMETER
#ifndef MAGNETOMETER
# define MAGNETOMETER			DISABLED
#endif
#ifndef MAG_ORIENTATION
# define MAG_ORIENTATION		AP_COMPASS_COMPONENTS_DOWN_PINS_FORWARD
#endif


//////////////////////////////////////////////////////////////////////////////
//  CAMERA GAINS
#ifndef CAM_ROLL_GAIN
# define CAM_ROLL_GAIN			1.0
#endif
#ifndef CAM_PITCH_GAIN
# define CAM_PITCH_GAIN			1.0
#endif


//////////////////////////////////////////////////////////////////////////////
//  OPTICAL_FLOW
#if defined( __AVR_ATmega2560__ )  // determines if optical flow code is included
  //#define OPTFLOW_ENABLED
#endif

#ifndef OPTFLOW					// sets global enabled/disabled flag for optflow (as seen in CLI)
# define OPTFLOW				DISABLED
#endif
#ifndef OPTFLOW_ORIENTATION
# define OPTFLOW_ORIENTATION 	AP_OPTICALFLOW_ADNS3080_PINS_FORWARD
#endif
#ifndef OPTFLOW_FOV
# define OPTFLOW_FOV 			AP_OPTICALFLOW_ADNS3080_08_FOV
#endif

//////////////////////////////////////////////////////////////////////////////
// RADIO CONFIGURATION
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// FLIGHT_MODE
//

#if !defined(FLIGHT_MODE_1)
# define FLIGHT_MODE_1			STABILIZE
#endif
#if !defined(FLIGHT_MODE_2)
# define FLIGHT_MODE_2			STABILIZE
#endif
#if !defined(FLIGHT_MODE_3)
# define FLIGHT_MODE_3			STABILIZE
#endif
#if !defined(FLIGHT_MODE_4)
# define FLIGHT_MODE_4			STABILIZE
#endif
#if !defined(FLIGHT_MODE_5)
# define FLIGHT_MODE_5			STABILIZE
#endif
#if !defined(FLIGHT_MODE_6)
# define FLIGHT_MODE_6			STABILIZE
#endif


//////////////////////////////////////////////////////////////////////////////
// THROTTLE_FAILSAFE
// THROTTLE_FS_VALUE
// THROTTLE_FAILSAFE_ACTION
//
#ifndef THROTTLE_FAILSAFE
# define THROTTLE_FAILSAFE			DISABLED
#endif
#ifndef THROTTE_FS_VALUE
# define THROTTLE_FS_VALUE			975
#endif
#ifndef THROTTLE_FAILSAFE_ACTION
# define THROTTLE_FAILSAFE_ACTION	2
#endif


#ifndef MINIMUM_THROTTLE
# define MINIMUM_THROTTLE	130
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// STARTUP BEHAVIOUR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// GROUND_START_DELAY
//
#ifndef GROUND_START_DELAY
# define GROUND_START_DELAY		3
#endif


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// FLIGHT AND NAVIGATION CONTROL
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// Y6 Support

#ifndef TOP_BOTTOM_RATIO
# define TOP_BOTTOM_RATIO	1.00
#endif


//////////////////////////////////////////////////////////////////////////////
// Attitude Control
//

// Alt Hold Mode
#ifndef ALT_HOLD_YAW
# define ALT_HOLD_YAW 		YAW_HOLD
#endif

#ifndef ALT_HOLD_RP
# define ALT_HOLD_RP 		ROLL_PITCH_STABLE
#endif

#ifndef ALT_HOLD_THR
# define ALT_HOLD_THR		THROTTLE_HOLD
#endif

// AUTO Mode
#ifndef AUTO_YAW
# define AUTO_YAW 			YAW_AUTO
#endif

#ifndef AUTO_RP
# define AUTO_RP 			ROLL_PITCH_AUTO
#endif

#ifndef AUTO_THR
# define AUTO_THR			THROTTLE_AUTO
#endif

// CIRCLE Mode
#ifndef CIRCLE_YAW
# define CIRCLE_YAW 		YAW_AUTO
#endif

#ifndef CIRCLE_RP
# define CIRCLE_RP 			ROLL_PITCH_AUTO
#endif

#ifndef CIRCLE_THR
# define CIRCLE_THR			THROTTLE_HOLD
#endif

// LOITER Mode
#ifndef LOITER_YAW
# define LOITER_YAW 		YAW_HOLD
#endif

#ifndef LOITER_RP
# define LOITER_RP 			ROLL_PITCH_AUTO
#endif

#ifndef LOITER_THR
# define LOITER_THR			THROTTLE_HOLD
#endif


// RTL Mode
#ifndef RTL_YAW
# define RTL_YAW 			YAW_HOLD
#endif

#ifndef RTL_RP
# define RTL_RP 			ROLL_PITCH_AUTO
#endif

#ifndef RTL_THR
# define RTL_THR			THROTTLE_HOLD
#endif




//////////////////////////////////////////////////////////////////////////////
// Attitude Control
//

// Extra motor values that are changed from time to time by jani @ jDrones as software
// and charachteristics changes.
#ifdef MOTORS_JD880
# define STABILIZE_ROLL_P 		3.6
# define STABILIZE_ROLL_I 		0.06
# define STABILIZE_ROLL_IMAX 	        2.0		// degrees
# define STABILIZE_PITCH_P		3.6
# define STABILIZE_PITCH_I		0.06
# define STABILIZE_PITCH_IMAX	        2.0		// degrees
#endif

// Jasons default values that are good for smaller payload motors.
#ifndef STABILIZE_ROLL_P
# define STABILIZE_ROLL_P 		4.6
#endif
#ifndef STABILIZE_ROLL_I
# define STABILIZE_ROLL_I 		0.0
#endif
#ifndef STABILIZE_ROLL_IMAX
# define STABILIZE_ROLL_IMAX 	1.5		// degrees
#endif

#ifndef STABILIZE_PITCH_P
# define STABILIZE_PITCH_P		4.6
#endif
#ifndef STABILIZE_PITCH_I
# define STABILIZE_PITCH_I		0.0
#endif
#ifndef STABILIZE_PITCH_IMAX
# define STABILIZE_PITCH_IMAX	1.5		// degrees
#endif

//////////////////////////////////////////////////////////////////////////////
// Acro Rate Control
//
#ifndef ACRO_ROLL_P
# define ACRO_ROLL_P         0.145
#endif
#ifndef ACRO_ROLL_I
# define ACRO_ROLL_I         0.0
#endif
#ifndef ACRO_ROLL_IMAX
# define ACRO_ROLL_IMAX	 	15			// degrees
#endif

#ifndef ACRO_PITCH_P
# define ACRO_PITCH_P       0.145
#endif
#ifndef ACRO_PITCH_I
# define ACRO_PITCH_I		0 //0.18
#endif
#ifndef ACRO_PITCH_IMAX
# define ACRO_PITCH_IMAX   	15			// degrees
#endif

//////////////////////////////////////////////////////////////////////////////
// Stabilize Rate Control
//
#ifndef RATE_ROLL_P
# define RATE_ROLL_P         0.145
#endif
#ifndef RATE_ROLL_I
# define RATE_ROLL_I         0.0
#endif
#ifndef RATE_ROLL_IMAX
# define RATE_ROLL_IMAX	 	15			// degrees
#endif

#ifndef RATE_PITCH_P
# define RATE_PITCH_P       0.145
#endif
#ifndef RATE_PITCH_I
# define RATE_PITCH_I		0 //0.18
#endif
#ifndef RATE_PITCH_IMAX
# define RATE_PITCH_IMAX   	15			// degrees
#endif

//////////////////////////////////////////////////////////////////////////////
// YAW Control
//
#ifndef  STABILIZE_YAW_P
# define STABILIZE_YAW_P		7			// increase for more aggressive Yaw Hold, decrease if it's bouncy
#endif
#ifndef  STABILIZE_YAW_I
# define STABILIZE_YAW_I		0.01		// set to .0001 to try and get over user's steady state error caused by poor balance
#endif
#ifndef  STABILIZE_YAW_IMAX
# define STABILIZE_YAW_IMAX		8			// degrees * 100
#endif

#ifndef RATE_YAW_P
# define RATE_YAW_P     .13			// used to control response in turning
#endif
#ifndef RATE_YAW_I
# define RATE_YAW_I     0.0
#endif
#ifndef RATE_YAW_IMAX
# define RATE_YAW_IMAX   50
#endif


//////////////////////////////////////////////////////////////////////////////
// Autopilot control limits
//
// how much to we pitch towards the target
#ifndef PITCH_MAX
# define PITCH_MAX				22			// degrees
#endif


//////////////////////////////////////////////////////////////////////////////
// Navigation control gains
//
#ifndef LOITER_P
# define LOITER_P			.3		//
#endif
#ifndef LOITER_I
# define LOITER_I			0.0	//
#endif
#ifndef LOITER_IMAX
# define LOITER_IMAX		12		// degrees°
#endif

#ifndef NAV_P
# define NAV_P				3.0			//
#endif
#ifndef NAV_I
# define NAV_I				0.05		// Lowerd from .25 - saw lots of overshoot.
#endif
#ifndef NAV_IMAX
# define NAV_IMAX			20			// degrees
#endif

#ifndef WAYPOINT_SPEED_MAX
# define WAYPOINT_SPEED_MAX			400			// for 6m/s error = 13mph
#endif


//////////////////////////////////////////////////////////////////////////////
// Throttle control gains
//
#ifndef THROTTLE_CRUISE
# define THROTTLE_CRUISE	350			//
#endif

#ifndef THR_HOLD_P
# define THR_HOLD_P		0.4			//
#endif
#ifndef THR_HOLD_I
# define THR_HOLD_I		0.02		// with 4m error, 12.5s windup
#endif
#ifndef THR_HOLD_IMAX
# define THR_HOLD_IMAX	300
#endif

// RATE control
#ifndef THROTTLE_P
# define THROTTLE_P		0.5			//
#endif
#ifndef THROTTLE_I
# define THROTTLE_I		0.0			//
#endif
#ifndef THROTTLE_IMAX
# define THROTTLE_IMAX	50
#endif


//////////////////////////////////////////////////////////////////////////////
// Crosstrack compensation
//
#ifndef CROSSTRACK_GAIN
# define CROSSTRACK_GAIN		4
#endif


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// DEBUGGING
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// DEBUG_LEVEL
//
#ifndef DEBUG_LEVEL
# define DEBUG_LEVEL SEVERITY_LOW
#endif


//////////////////////////////////////////////////////////////////////////////
// Dataflash logging control
//

#ifndef LOGGING_ENABLED
# define LOGGING_ENABLED               ENABLED
#endif

#ifndef LOG_ATTITUDE_FAST
# define LOG_ATTITUDE_FAST		DISABLED
#endif
#ifndef LOG_ATTITUDE_MED
# define LOG_ATTITUDE_MED 		ENABLED
#endif
#ifndef LOG_GPS
# define LOG_GPS 				ENABLED
#endif
#ifndef LOG_PM
# define LOG_PM 				ENABLED
#endif
#ifndef LOG_CTUN
# define LOG_CTUN				ENABLED
#endif
#ifndef LOG_NTUN
# define LOG_NTUN				ENABLED
#endif
#ifndef LOG_MODE
# define LOG_MODE				ENABLED
#endif
#ifndef LOG_RAW
# define LOG_RAW				DISABLED
#endif
#ifndef LOG_CMD
# define LOG_CMD				ENABLED
#endif
// current
#ifndef LOG_CUR
# define LOG_CUR				DISABLED
#endif
// quad motor PWMs
#ifndef LOG_MOTORS
# define LOG_MOTORS				DISABLED
#endif
// guess!
#ifndef LOG_OPTFLOW
# define LOG_OPTFLOW				DISABLED
#endif

// calculate the default log_bitmask
#define LOGBIT(_s)     (LOG_##_s ? MASK_LOG_##_s : 0)

#define DEFAULT_LOG_BITMASK \
               LOGBIT(ATTITUDE_FAST)   | \
               LOGBIT(ATTITUDE_MED)    | \
               LOGBIT(GPS)                             | \
               LOGBIT(PM)                              | \
               LOGBIT(CTUN)                    | \
               LOGBIT(NTUN)                    | \
               LOGBIT(MODE)                    | \
               LOGBIT(RAW)                             | \
               LOGBIT(CMD)                             | \
               LOGBIT(CUR)						| \
               LOGBIT(MOTORS)					| \
               LOGBIT(OPTFLOW)

// if we are using fast, Disable Medium
//#if LOG_ATTITUDE_FAST == ENABLED
//	#undef LOG_ATTITUDE_MED
//	#define LOG_ATTITUDE_MED 		DISABLED
//#endif

#ifndef DEBUG_PORT
# define DEBUG_PORT 0
#endif

#if DEBUG_PORT == 0
# define SendDebug_P(a) Serial.print_P(PSTR(a))
# define SendDebugln_P(a) Serial.println_P(PSTR(a))
# define SendDebug Serial.print
# define SendDebugln Serial.println
#elif DEBUG_PORT == 1
# define SendDebug_P(a) Serial1.print_P(PSTR(a))
# define SendDebugln_P(a) Serial1.println_P(PSTR(a))
# define SendDebug Serial1.print
# define SendDebugln Serial1.println
#elif DEBUG_PORT == 2
# define SendDebug_P(a) Serial2.print_P(PSTR(a))
# define SendDebugln_P(a) Serial2.println_P(PSTR(a))
# define SendDebug Serial2.print
# define SendDebugln Serial2.println
#elif DEBUG_PORT == 3
# define SendDebug_P(a) Serial3.print_P(PSTR(a))
# define SendDebugln_P(a) Serial3.println_P(PSTR(a))
# define SendDebug Serial3.print
# define SendDebugln Serial3.println
#endif

//////////////////////////////////////////////////////////////////////////////
// Navigation defaults
//
#ifndef WP_RADIUS_DEFAULT
# define WP_RADIUS_DEFAULT		3
#endif

#ifndef LOITER_RADIUS
# define LOITER_RADIUS 10		// meters for circle mode
#endif

#ifndef ALT_HOLD_HOME
# define ALT_HOLD_HOME 0		// height to return to Home, 0 = Maintain current altitude
#endif

#ifndef USE_CURRENT_ALT
# define USE_CURRENT_ALT FALSE
#endif


#ifndef AUTO_RESET_LOITER
# define AUTO_RESET_LOITER	1	// enables Loiter to reset it's current location based on stick input.
#endif
#ifndef CUT_MOTORS
# define CUT_MOTORS		1		// do we cut the motors with no throttle?
#endif

#ifndef MOTOR_LEDS
# define MOTOR_LEDS		1		// 0 = off, 1 = on
#endif


//////////////////////////////////////////////////////////////////////////////
// RC override
//
#ifndef ALLOW_RC_OVERRIDE
# define ALLOW_RC_OVERRIDE DISABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// Developer Items
//

// use this to completely disable the CLI
#ifndef CLI_ENABLED
# define CLI_ENABLED ENABLED
#endif

// use this to disable the CLI slider switch
#ifndef CLI_SLIDER_ENABLED
# define CLI_SLIDER_ENABLED ENABLED
#endif

// delay to prevent Xbee bricking, in milliseconds
#ifndef MAVLINK_TELEMETRY_PORT_DELAY
# define MAVLINK_TELEMETRY_PORT_DELAY 2000
#endif
