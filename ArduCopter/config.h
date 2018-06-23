//
#pragma once

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
#include "APM_Config.h"


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// HARDWARE CONFIGURATION AND CONNECTIONS
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef CONFIG_HAL_BOARD
#error CONFIG_HAL_BOARD must be defined to build ArduCopter
#endif

//////////////////////////////////////////////////////////////////////////////
// HIL_MODE                                 OPTIONAL

#ifndef HIL_MODE
 #define HIL_MODE        HIL_MODE_DISABLED
#endif

#define MAGNETOMETER ENABLED

#ifndef ARMING_DELAY_SEC
    # define ARMING_DELAY_SEC 2.0f
#endif

//////////////////////////////////////////////////////////////////////////////
// FRAME_CONFIG
//
#ifndef FRAME_CONFIG
 # define FRAME_CONFIG   MULTICOPTER_FRAME
#endif

/////////////////////////////////////////////////////////////////////////////////
// TradHeli defaults
#if FRAME_CONFIG == HELI_FRAME
  # define RC_FAST_SPEED                        125
  # define WP_YAW_BEHAVIOR_DEFAULT              WP_YAW_BEHAVIOR_LOOK_AHEAD
  # define THR_MIN_DEFAULT                      0
  # define AUTOTUNE_ENABLED                     DISABLED
  # define ACCEL_Z_P                            0.30f
#endif

//////////////////////////////////////////////////////////////////////////////
// PWM control
// default RC speed in Hz
#ifndef RC_FAST_SPEED
   #   define RC_FAST_SPEED 490
#endif

//////////////////////////////////////////////////////////////////////////////
// Rangefinder
//

#ifndef RANGEFINDER_ENABLED
 # define RANGEFINDER_ENABLED ENABLED
#endif

#ifndef RANGEFINDER_HEALTH_MAX
 # define RANGEFINDER_HEALTH_MAX 3          // number of good reads that indicates a healthy rangefinder
#endif

#ifndef RANGEFINDER_GAIN_DEFAULT
 # define RANGEFINDER_GAIN_DEFAULT 0.8f     // gain for controlling how quickly rangefinder range adjusts target altitude (lower means slower reaction)
#endif

#ifndef THR_SURFACE_TRACKING_VELZ_MAX
 # define THR_SURFACE_TRACKING_VELZ_MAX 150 // max vertical speed change while surface tracking with rangefinder
#endif

#ifndef RANGEFINDER_TIMEOUT_MS
 # define RANGEFINDER_TIMEOUT_MS  1000      // desired rangefinder alt will reset to current rangefinder alt after this many milliseconds without a good rangefinder alt
#endif

#ifndef RANGEFINDER_WPNAV_FILT_HZ
 # define RANGEFINDER_WPNAV_FILT_HZ   0.25f // filter frequency for rangefinder altitude provided to waypoint navigation class
#endif

#ifndef RANGEFINDER_TILT_CORRECTION         // by disable tilt correction for use of range finder data by EKF
 # define RANGEFINDER_TILT_CORRECTION ENABLED
#endif

#ifndef RANGEFINDER_GLITCH_ALT_CM
 # define RANGEFINDER_GLITCH_ALT_CM  200      // amount of rangefinder change to be considered a glitch
#endif

#ifndef RANGEFINDER_GLITCH_NUM_SAMPLES
 # define RANGEFINDER_GLITCH_NUM_SAMPLES  3   // number of rangefinder glitches in a row to take new reading
#endif

//////////////////////////////////////////////////////////////////////////////
// Proximity sensor
//
#ifndef PROXIMITY_ENABLED
 # define PROXIMITY_ENABLED ENABLED
#endif

#ifndef MAV_SYSTEM_ID
 # define MAV_SYSTEM_ID          1
#endif


//////////////////////////////////////////////////////////////////////////////
// Battery monitoring
//
#ifndef BOARD_VOLTAGE_MIN
 # define BOARD_VOLTAGE_MIN             4.3f        // min board voltage in volts for pre-arm checks
#endif

#ifndef BOARD_VOLTAGE_MAX
 # define BOARD_VOLTAGE_MAX             5.8f        // max board voltage in volts for pre-arm checks
#endif

// prearm GPS hdop check
#ifndef GPS_HDOP_GOOD_DEFAULT
 # define GPS_HDOP_GOOD_DEFAULT         140     // minimum hdop that represents a good position.  used during pre-arm checks if fence is enabled
#endif

// GCS failsafe
#ifndef FS_GCS
 # define FS_GCS                        DISABLED
#endif
#ifndef FS_GCS_TIMEOUT_MS
 # define FS_GCS_TIMEOUT_MS             5000    // gcs failsafe triggers after 5 seconds with no GCS heartbeat
#endif

// Radio failsafe while using RC_override
#ifndef FS_RADIO_RC_OVERRIDE_TIMEOUT_MS
 # define FS_RADIO_RC_OVERRIDE_TIMEOUT_MS  1000    // RC Radio failsafe triggers after 1 second while using RC_override from ground station
#endif

// Radio failsafe
#ifndef FS_RADIO_TIMEOUT_MS
 #define FS_RADIO_TIMEOUT_MS            500     // RC Radio Failsafe triggers after 500 miliseconds with No RC Input
#endif

// missing terrain data failsafe
#ifndef FS_TERRAIN_TIMEOUT_MS
 #define FS_TERRAIN_TIMEOUT_MS          5000     // 5 seconds of missing terrain data will trigger failsafe (RTL)
#endif

#ifndef PREARM_DISPLAY_PERIOD
# define PREARM_DISPLAY_PERIOD 30
#endif

// pre-arm baro vs inertial nav max alt disparity
#ifndef PREARM_MAX_ALT_DISPARITY_CM
 # define PREARM_MAX_ALT_DISPARITY_CM       100     // barometer and inertial nav altitude must be within this many centimeters
#endif

//////////////////////////////////////////////////////////////////////////////
//  EKF Failsafe
#ifndef FS_EKF_ACTION_DEFAULT
 # define FS_EKF_ACTION_DEFAULT         FS_EKF_ACTION_LAND  // EKF failsafe triggers land by default
#endif
#ifndef FS_EKF_THRESHOLD_DEFAULT
 # define FS_EKF_THRESHOLD_DEFAULT      0.8f    // EKF failsafe's default compass and velocity variance threshold above which the EKF failsafe will be triggered
#endif

#ifndef EKF_ORIGIN_MAX_DIST_M
 # define EKF_ORIGIN_MAX_DIST_M         50000   // EKF origin and waypoints (including home) must be within 50km
#endif

//////////////////////////////////////////////////////////////////////////////
//  MAGNETOMETER
#ifndef MAGNETOMETER
 # define MAGNETOMETER                   ENABLED
#endif

#ifndef COMPASS_CAL_STICK_GESTURE_TIME
 #define COMPASS_CAL_STICK_GESTURE_TIME 2.0f // 2 seconds
#endif
#ifndef COMPASS_CAL_STICK_DELAY
 #define COMPASS_CAL_STICK_DELAY 5.0f
#endif

//////////////////////////////////////////////////////////////////////////////
//  OPTICAL_FLOW & VISUAL ODOMETRY
#ifndef OPTFLOW
 # define OPTFLOW       ENABLED
#endif
#ifndef VISUAL_ODOMETRY_ENABLED
# define VISUAL_ODOMETRY_ENABLED !HAL_MINIMIZE_FEATURES
#endif

//////////////////////////////////////////////////////////////////////////////
//  Auto Tuning
#ifndef AUTOTUNE_ENABLED
 # define AUTOTUNE_ENABLED  ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
//  Crop Sprayer - enabled only on larger firmwares
#ifndef SPRAYER_ENABLED
 # define SPRAYER_ENABLED  !HAL_MINIMIZE_FEATURES
#endif

//////////////////////////////////////////////////////////////////////////////
// Precision Landing with companion computer or IRLock sensor
#ifndef PRECISION_LANDING
 # define PRECISION_LANDING ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// gripper - enabled only on larger firmwares
#ifndef GRIPPER_ENABLED
 # define GRIPPER_ENABLED !HAL_MINIMIZE_FEATURES
#endif

//////////////////////////////////////////////////////////////////////////////
// winch support - enabled only on larger firmwares
#ifndef WINCH_ENABLED
# define WINCH_ENABLED !HAL_MINIMIZE_FEATURES
#endif

//////////////////////////////////////////////////////////////////////////////
// rotations per minute sensor support
#ifndef RPM_ENABLED
# define RPM_ENABLED ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// Parachute release
#ifndef PARACHUTE
 # define PARACHUTE ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// ADSB support
#ifndef ADSB_ENABLED
# define ADSB_ENABLED !HAL_MINIMIZE_FEATURES
#endif

//////////////////////////////////////////////////////////////////////////////
// Nav-Guided - allows external nav computer to control vehicle
#ifndef NAV_GUIDED
 # define NAV_GUIDED    !HAL_MINIMIZE_FEATURES
#endif

//////////////////////////////////////////////////////////////////////////////
// Acro - fly vehicle in acrobatic mode
#ifndef MODE_ACRO_ENABLED
# define MODE_ACRO_ENABLED ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// Auto mode - allows vehicle to trace waypoints and perform automated actions
#ifndef MODE_AUTO_ENABLED
# define MODE_AUTO_ENABLED ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// Brake mode - bring vehicle to stop
#ifndef MODE_BRAKE_ENABLED
# define MODE_BRAKE_ENABLED ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// Circle - fly vehicle around a central point
#ifndef MODE_CIRCLE_ENABLED
# define MODE_CIRCLE_ENABLED ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// Drift - fly vehicle in altitude-held, coordinated-turn mode
#ifndef MODE_DRIFT_ENABLED
# define MODE_DRIFT_ENABLED ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// Follow - follow another vehicle or GCS
#ifndef MODE_FOLLOW_ENABLED
# define MODE_FOLLOW_ENABLED !HAL_MINIMIZE_FEATURES
#endif

//////////////////////////////////////////////////////////////////////////////
// Guided mode - control vehicle's position or angles from GCS
#ifndef MODE_GUIDED_ENABLED
# define MODE_GUIDED_ENABLED ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// GuidedNoGPS mode - control vehicle's angles from GCS
#ifndef MODE_GUIDED_NOGPS_ENABLED
# define MODE_GUIDED_NOGPS_ENABLED ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// Loiter mode - allows vehicle to hold global position
#ifndef MODE_LOITER_ENABLED
# define MODE_LOITER_ENABLED ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// Position Hold - enable holding of global position
#ifndef MODE_POSHOLD_ENABLED
# define MODE_POSHOLD_ENABLED ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// RTL - Return To Launch
#ifndef MODE_RTL_ENABLED
# define MODE_RTL_ENABLED ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// SmartRTL - allows vehicle to retrace a (loop-eliminated) breadcrumb home
#ifndef MODE_SMARTRTL_ENABLED
# define MODE_SMARTRTL_ENABLED ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// Sport - fly vehicle in rate-controlled (earth-frame) mode
#ifndef MODE_SPORT_ENABLED
# define MODE_SPORT_ENABLED !HAL_MINIMIZE_FEATURES
#endif

//////////////////////////////////////////////////////////////////////////////
// Throw - fly vehicle after throwing it in the air
#ifndef MODE_THROW_ENABLED
# define MODE_THROW_ENABLED ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// Beacon support - support for local positioning systems
#ifndef BEACON_ENABLED
# define BEACON_ENABLED !HAL_MINIMIZE_FEATURES
#endif

//////////////////////////////////////////////////////////////////////////////
// RADIO CONFIGURATION
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// FLIGHT_MODE
//

#ifndef FLIGHT_MODE_1
 # define FLIGHT_MODE_1                  STABILIZE
#endif
#ifndef FLIGHT_MODE_2
 # define FLIGHT_MODE_2                  STABILIZE
#endif
#ifndef FLIGHT_MODE_3
 # define FLIGHT_MODE_3                  STABILIZE
#endif
#ifndef FLIGHT_MODE_4
 # define FLIGHT_MODE_4                  STABILIZE
#endif
#ifndef FLIGHT_MODE_5
 # define FLIGHT_MODE_5                  STABILIZE
#endif
#ifndef FLIGHT_MODE_6
 # define FLIGHT_MODE_6                  STABILIZE
#endif


//////////////////////////////////////////////////////////////////////////////
// Throttle Failsafe
//
#ifndef FS_THR_VALUE_DEFAULT
 # define FS_THR_VALUE_DEFAULT             975
#endif

//////////////////////////////////////////////////////////////////////////////
// Takeoff
//
#ifndef PILOT_TKOFF_ALT_DEFAULT
 # define PILOT_TKOFF_ALT_DEFAULT           0     // default final alt above home for pilot initiated takeoff
#endif


//////////////////////////////////////////////////////////////////////////////
// Landing
//
#ifndef LAND_SPEED
 # define LAND_SPEED    50          // the descent speed for the final stage of landing in cm/s
#endif
#ifndef LAND_REPOSITION_DEFAULT
 # define LAND_REPOSITION_DEFAULT   1   // by default the pilot can override roll/pitch during landing
#endif
#ifndef LAND_WITH_DELAY_MS
 # define LAND_WITH_DELAY_MS        4000    // default delay (in milliseconds) when a land-with-delay is triggered during a failsafe event
#endif
#ifndef LAND_CANCEL_TRIGGER_THR
 # define LAND_CANCEL_TRIGGER_THR   700     // land is cancelled by input throttle above 700
#endif
#ifndef LAND_RANGEFINDER_MIN_ALT_CM
#define LAND_RANGEFINDER_MIN_ALT_CM 200
#endif

//////////////////////////////////////////////////////////////////////////////
// Landing Detector
//
#ifndef LAND_DETECTOR_TRIGGER_SEC
 # define LAND_DETECTOR_TRIGGER_SEC         1.0f    // number of seconds to detect a landing
#endif
#ifndef LAND_DETECTOR_MAYBE_TRIGGER_SEC
 # define LAND_DETECTOR_MAYBE_TRIGGER_SEC   0.2f    // number of seconds that means we might be landed (used to reset horizontal position targets to prevent tipping over)
#endif
#ifndef LAND_DETECTOR_ACCEL_LPF_CUTOFF
# define LAND_DETECTOR_ACCEL_LPF_CUTOFF     1.0f    // frequency cutoff of land detector accelerometer filter
#endif
#ifndef LAND_DETECTOR_ACCEL_MAX
# define LAND_DETECTOR_ACCEL_MAX            1.0f    // vehicle acceleration must be under 1m/s/s
#endif

//////////////////////////////////////////////////////////////////////////////
// CAMERA TRIGGER AND CONTROL
//
#ifndef CAMERA
 # define CAMERA        ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// MOUNT (ANTENNA OR CAMERA)
//
#ifndef MOUNT
 # define MOUNT         ENABLED
#endif


//////////////////////////////////////////////////////////////////////////////
// Flight mode definitions
//

// Acro Mode
#ifndef ACRO_RP_P
 # define ACRO_RP_P                 4.5f
#endif

#ifndef ACRO_YAW_P
 # define ACRO_YAW_P                4.5f
#endif

#ifndef ACRO_LEVEL_MAX_ANGLE
 # define ACRO_LEVEL_MAX_ANGLE      3000
#endif

#ifndef ACRO_BALANCE_ROLL
 #define ACRO_BALANCE_ROLL          1.0f
#endif

#ifndef ACRO_BALANCE_PITCH
 #define ACRO_BALANCE_PITCH         1.0f
#endif

#ifndef ACRO_RP_EXPO_DEFAULT
 #define ACRO_RP_EXPO_DEFAULT       0.3f
#endif

#ifndef ACRO_Y_EXPO_DEFAULT
 #define ACRO_Y_EXPO_DEFAULT        0.0f
#endif

#ifndef ACRO_THR_MID_DEFAULT
 #define ACRO_THR_MID_DEFAULT       0.0f
#endif

// RTL Mode
#ifndef RTL_ALT_FINAL
 # define RTL_ALT_FINAL             0       // the altitude the vehicle will move to as the final stage of Returning to Launch.  Set to zero to land.
#endif

#ifndef RTL_ALT
 # define RTL_ALT 				    1500    // default alt to return to home in cm, 0 = Maintain current altitude
#endif

#ifndef RTL_ALT_MIN
 # define RTL_ALT_MIN               200     // min height above ground for RTL (i.e 2m)
#endif

#ifndef RTL_CLIMB_MIN_DEFAULT
 # define RTL_CLIMB_MIN_DEFAULT     0       // vehicle will always climb this many cm as first stage of RTL
#endif

#ifndef RTL_ABS_MIN_CLIMB
 # define RTL_ABS_MIN_CLIMB         250     // absolute minimum initial climb
#endif

#ifndef RTL_CONE_SLOPE_DEFAULT
 # define RTL_CONE_SLOPE_DEFAULT    3.0f    // slope of RTL cone (height / distance). 0 = No cone
#endif

#ifndef RTL_MIN_CONE_SLOPE
 # define RTL_MIN_CONE_SLOPE        0.5f    // minimum slope of cone
#endif

#ifndef RTL_LOITER_TIME
 # define RTL_LOITER_TIME           5000    // Time (in milliseconds) to loiter above home before beginning final descent
#endif

// AUTO Mode
#ifndef WP_YAW_BEHAVIOR_DEFAULT
 # define WP_YAW_BEHAVIOR_DEFAULT   WP_YAW_BEHAVIOR_LOOK_AT_NEXT_WP_EXCEPT_RTL
#endif

#ifndef AUTO_YAW_SLEW_RATE
 # define AUTO_YAW_SLEW_RATE    60              // degrees/sec
#endif

#ifndef YAW_LOOK_AHEAD_MIN_SPEED
 # define YAW_LOOK_AHEAD_MIN_SPEED  100             // minimum ground speed in cm/s required before copter is aimed at ground course
#endif

// Super Simple mode
#ifndef SUPER_SIMPLE_RADIUS
 # define SUPER_SIMPLE_RADIUS       1000
#endif

//////////////////////////////////////////////////////////////////////////////
// Stabilize Rate Control
//
#ifndef ROLL_PITCH_YAW_INPUT_MAX
 # define ROLL_PITCH_YAW_INPUT_MAX      4500        // roll, pitch and yaw input range
#endif
#ifndef DEFAULT_ANGLE_MAX
 # define DEFAULT_ANGLE_MAX         4500            // ANGLE_MAX parameters default value
#endif
#ifndef ANGLE_RATE_MAX
 # define ANGLE_RATE_MAX            18000           // default maximum rotation rate in roll/pitch axis requested by angle controller used in stabilize, loiter, rtl, auto flight modes
#endif

//////////////////////////////////////////////////////////////////////////////
// Stop mode defaults
//
#ifndef BRAKE_MODE_SPEED_Z
 # define BRAKE_MODE_SPEED_Z     250 // z-axis speed in cm/s in Brake Mode
#endif
#ifndef BRAKE_MODE_DECEL_RATE
 # define BRAKE_MODE_DECEL_RATE  750 // acceleration rate in cm/s/s in Brake Mode
#endif

//////////////////////////////////////////////////////////////////////////////
// PosHold parameter defaults
//
#ifndef POSHOLD_BRAKE_RATE_DEFAULT
 # define POSHOLD_BRAKE_RATE_DEFAULT    8       // default POSHOLD_BRAKE_RATE param value.  Rotation rate during braking in deg/sec
#endif
#ifndef POSHOLD_BRAKE_ANGLE_DEFAULT
 # define POSHOLD_BRAKE_ANGLE_DEFAULT   3000    // default POSHOLD_BRAKE_ANGLE param value.  Max lean angle during braking in centi-degrees
#endif

//////////////////////////////////////////////////////////////////////////////
// Throttle control defaults
//

#ifndef THR_DZ_DEFAULT
# define THR_DZ_DEFAULT         100             // the deadzone above and below mid throttle while in althold or loiter
#endif

// default maximum vertical velocity and acceleration the pilot may request
#ifndef PILOT_VELZ_MAX
 # define PILOT_VELZ_MAX    250     // maximum vertical velocity in cm/s
#endif
#ifndef PILOT_ACCEL_Z_DEFAULT
 # define PILOT_ACCEL_Z_DEFAULT 250 // vertical acceleration in cm/s/s while altitude is under pilot control
#endif

// max distance in cm above or below current location that will be used for the alt target when transitioning to alt-hold mode
#ifndef ALT_HOLD_INIT_MAX_OVERSHOOT
 # define ALT_HOLD_INIT_MAX_OVERSHOOT 200
#endif
// the acceleration used to define the distance-velocity curve
#ifndef ALT_HOLD_ACCEL_MAX
 # define ALT_HOLD_ACCEL_MAX 250    // if you change this you must also update the duplicate declaration in AC_WPNav.h
#endif

#ifndef AUTO_DISARMING_DELAY
# define AUTO_DISARMING_DELAY  10
#endif

//////////////////////////////////////////////////////////////////////////////
// Throw mode configuration
//
#ifndef THROW_HIGH_SPEED
# define THROW_HIGH_SPEED       500.0f  // vehicle much reach this total 3D speed in cm/s (or be free falling)
#endif
#ifndef THROW_VERTICAL_SPEED
# define THROW_VERTICAL_SPEED   50.0f   // motors start when vehicle reaches this total 3D speed in cm/s
#endif

//////////////////////////////////////////////////////////////////////////////
// Dataflash logging control
//
#ifndef LOGGING_ENABLED
 # define LOGGING_ENABLED                ENABLED
#endif

// Default logging bitmask
#ifndef DEFAULT_LOG_BITMASK
 # define DEFAULT_LOG_BITMASK \
    MASK_LOG_ATTITUDE_MED | \
    MASK_LOG_GPS | \
    MASK_LOG_PM | \
    MASK_LOG_CTUN | \
    MASK_LOG_NTUN | \
    MASK_LOG_RCIN | \
    MASK_LOG_IMU | \
    MASK_LOG_CMD | \
    MASK_LOG_CURRENT | \
    MASK_LOG_RCOUT | \
    MASK_LOG_OPTFLOW | \
    MASK_LOG_COMPASS | \
    MASK_LOG_CAMERA | \
    MASK_LOG_MOTBATT
#endif

//////////////////////////////////////////////////////////////////////////////
// Fence, Rally and Terrain and AC_Avoidance defaults
//

// Enable/disable Fence
#ifndef AC_FENCE
 #define AC_FENCE ENABLED
#endif

#ifndef AC_RALLY
 #define AC_RALLY   ENABLED
#endif

#ifndef AC_TERRAIN
 #define AC_TERRAIN ENABLED
#endif

#if AC_TERRAIN && !AC_RALLY
 #error Terrain relies on Rally which is disabled
#endif

#ifndef AC_AVOID_ENABLED
 #define AC_AVOID_ENABLED   ENABLED
#endif

#if AC_AVOID_ENABLED && !PROXIMITY_ENABLED
  #error AC_Avoidance relies on PROXIMITY_ENABLED which is disabled
#endif
#if AC_AVOID_ENABLED && !AC_FENCE
  #error AC_Avoidance relies on AC_FENCE which is disabled
#endif

#if MODE_FOLLOW_ENABLED && !AC_AVOID_ENABLED
  #error Follow Mode relies on AC_AVOID which is disabled
#endif

#if MODE_AUTO_ENABLED && !MODE_GUIDED_ENABLED
  #error ModeAuto requires ModeGuided which is disabled
#endif

#if MODE_AUTO_ENABLED && !MODE_CIRCLE_ENABLED
  #error ModeAuto requires ModeCircle which is disabled
#endif

#if MODE_AUTO_ENABLED && !MODE_RTL_ENABLED
  #error ModeAuto requires ModeRTL which is disabled
#endif

#if AC_TERRAIN && !MODE_AUTO_ENABLED
  #error Terrain requires ModeAuto which is disabled
#endif

#if FRAME_CONFIG == HELI_FRAME && !MODE_ACRO_ENABLED
  #error Helicopter frame requires acro mode support which is disabled
#endif

//////////////////////////////////////////////////////////////////////////////
// Developer Items
//

//use this to completely disable FRSKY TELEM
#ifndef FRSKY_TELEM_ENABLED
  #  define FRSKY_TELEM_ENABLED          ENABLED
#endif

#ifndef ADVANCED_FAILSAFE
# define ADVANCED_FAILSAFE DISABLED
#endif

#ifndef CH_MODE_DEFAULT
 # define CH_MODE_DEFAULT   5
#endif

#ifndef TOY_MODE_ENABLED
#define TOY_MODE_ENABLED DISABLED
#endif

#if TOY_MODE_ENABLED && FRAME_CONFIG == HELI_FRAME
  #error Toy mode is not available on Helicopters
#endif

#ifndef STATS_ENABLED
 # define STATS_ENABLED ENABLED
#endif

#ifndef DEVO_TELEM_ENABLED
#if HAL_MINIMIZE_FEATURES
 #define DEVO_TELEM_ENABLED DISABLED
#else
 #define DEVO_TELEM_ENABLED ENABLED
#endif
#endif

#ifndef OSD_ENABLED
 #define OSD_ENABLED DISABLED
#endif
