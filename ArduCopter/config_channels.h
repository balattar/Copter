
#ifndef __ARDUCOPTER_CONFIG_MOTORS_H__
#define __ARDUCOPTER_CONFIG_MOTORS_H__

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

#include "config.h"     // Parent Config File
#include "APM_Config.h" // User Overrides

//
//
// Output CH mapping for ArduCopter motor channels
//
//
#if CONFIG_CHANNELS == CHANNEL_CONFIG_DEFAULT
# if CONFIG_APM_HARDWARE == APM_HARDWARE_APM2
#  define MOT_1 CH_1
#  define MOT_2 CH_2
#  define MOT_3 CH_3
#  define MOT_4 CH_4
#  define MOT_5 CH_5
#  define MOT_6 CH_6
#  define MOT_7 CH_7
#  define MOT_8 CH_8
# elif CONFIG_APM_HARDWARE == APM_HARDWARE_APM1
#  define MOT_1 CH_1
#  define MOT_2 CH_2
#  define MOT_3 CH_3
#  define MOT_4 CH_4
#  define MOT_5 CH_7
#  define MOT_6 CH_8
#  define MOT_7 CH_10
#  define MOT_8 CH_11
# endif 
#endif

//
//
// Output CH mapping for TRI_FRAME yaw servo
//
//
#if CONFIG_APM_HARDWARE == APM_HARDWARE_APM2
# define CH_TRI_YAW   CH_7
#elif CONFIG_APM_HARDWARE == APM_HARDWARE_APM1
# define CH_TRI_YAW   CH_7
#endif

//
//
// Output CH mapping for Aux channels
//
//
#if CONFIG_APM_HARDWARE == APM_HARDWARE_APM2
/* Camera Pitch and Camera Roll: Not yet defined for APM2
 * They will likely be dependent on the frame config */
# define CH_CAM_PITCH CH_11
# define CH_CAM_ROLL  CH_10
#elif CONFIG_APM_HARDWARE == APM_HARDWARE_APM1
# define CH_CAM_PITCH CH_5
# define CH_CAM_ROLL  CH_6
#endif

#endif // __ARDUCOPTER_CONFIG_MOTORS_H__
