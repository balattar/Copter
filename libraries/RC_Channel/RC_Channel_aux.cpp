// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: t -*-

#include <APM_RC.h>
#include "RC_Channel_aux.h"

int16_t
RC_Channel_aux::closest_limit(int16_t angle)
{
	// Change scaling to 0.1 degrees in order to avoid overflows in the angle arithmetic
	int16_t min = angle_min / 10;
	int16_t max = angle_max / 10;

	// Make sure the angle lies in the interval [-180 .. 180[ degrees
	while (angle < -1800) angle += 3600;
	while (angle >= 1800) angle -= 3600;

	// Make sure the angle limits lie in the interval [-180 .. 180[ degrees
	while (min < -1800) min += 3600;
	while (min >= 1800) min -= 3600;
	while (max < -1800) max += 3600;
	while (max >= 1800) max -= 3600;
	set_range(min, max);

	// If the angle is outside servo limits, saturate the angle to the closest limit
	// On a circle the closest angular position must be carefully calculated to account for wrap-around
	if ((angle < min) && (angle > max)){
		// angle error if min limit is used
		int16_t err_min = min - angle + (angle<min?0:3600); // add 360 degrees if on the "wrong side"
		// angle error if max limit is used
		int16_t err_max = angle - max + (angle>max?0:3600); // add 360 degrees if on the "wrong side"
		angle = err_min<err_max?min:max;
	}

	servo_out = angle;
	// convert angle to PWM using a linear transformation (ignores trimming because the camera limits might not be symmetric)
	calc_pwm();

	return angle;
}

// map a function to a servo channel and output it
void
RC_Channel_aux::output_ch(unsigned char ch_nr)
{
	switch(function)
	{
	case k_none: 		// disabled
		return;
		break;
	case k_manual:		// manual
		radio_out = radio_in;
		break;
	case k_flap:		// flaps
	case k_flap_auto:	// flaps automated
	case k_aileron:		// aileron
	case k_flaperon:	// flaperon (flaps and aileron combined, needs two independent servos one for each wing)
	case k_mount_yaw:	// mount yaw (pan)
	case k_mount_pitch:	// mount pitch (tilt)
	case k_mount_roll:	// mount roll
	case k_cam_trigger:	// camera trigger
	case k_cam_open:	// camera open
	case k_egg_drop:	// egg drop
	case k_nr_aux_servo_functions: // dummy, just to avoid a compiler warning
	default:
		break;
	}

	APM_RC.OutputCh(ch_nr, radio_out);
}
