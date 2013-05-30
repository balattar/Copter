// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

//	Code by Jon Challinger
//  Modified by Paul Riseborough
//
//	This library is free software; you can redistribute it and / or
//	modify it under the terms of the GNU Lesser General Public
//	License as published by the Free Software Foundation; either
//	version 2.1 of the License, or (at your option) any later version.

#include <AP_Math.h>
#include <AP_HAL.h>
#include "AP_RollController.h"

extern const AP_HAL::HAL& hal;

const AP_Param::GroupInfo AP_RollController::var_info[] PROGMEM = {
	// @Param: T_CONST
	// @DisplayName: Pitch Time Constant
	// @Description: This controls the time constant in seconds from demanded to achieved bank angle. A value of 0.7 is a good default and will work with nearly all models. Advanced users may want to reduce this time to obtain a faster response but there is no point setting a time less than the aircraft can achieve.
	// @Range: 0.4 1.0
	// @Units: seconds
	// @Increment: 0.1
	// @User: Advanced
	AP_GROUPINFO("T_CONST",      0, AP_RollController, _tau,       0.7),

	// @Param: K_P
	// @DisplayName: Proportional Gain
	// @Description: This is the gain from bank angle to aileron. This gain works the same way as the P term in the old PID (RLL2SRV_P) and can be set to the same value.
	// @Range: 0.1 1.0
	// @Increment: 0.1
	// @User: User
	AP_GROUPINFO("K_P",        1, AP_RollController, _K_P,          0.4),

	// @Param: K_D
	// @DisplayName: Damping Gain
	// @Description: This is the gain from roll rate to aileron. This adjusts the damping of the roll control loop. It has the same effect as RLL2SRV_D in the old PID controller but without the spikes in servo demands. This gain helps to reduce rolling in turbulence. It should be increased in 0.01 increments as too high a value can lead to a high frequency pitch oscillation that could overstress the airframe.
	// @Range: 0 0.1
	// @Increment: 0.01
	// @User: User
	AP_GROUPINFO("K_D",        2, AP_RollController, _K_D,        0.0),

	// @Param: K_I
	// @DisplayName: Integrator Gain
	// @Description: This is the gain from the integral of bank angle to aileron. It has the same effect as RLL2SRV_I in the old PID controller. Increasing this gain causes the controller to trim out steady offsets due to an out of trim aircraft.
	// @Range: 0 1.0
	// @Increment: 0.05
	// @User: User
	AP_GROUPINFO("K_I",        3, AP_RollController, _K_I,        0.0),

	// @Param: RMAX
	// @DisplayName: Maximum Roll Rate
	// @Description: This sets the maximum roll rate that the controller will demand (degrees/sec). Setting it to zero disables the limit. If this value is set too low, then the roll can't keep up with the navigation demands and the plane will start weaving. If it is set too high (or disabled by setting to zero) then ailerons will get large inputs at the start of turns. A limit of 60 degrees/sec is a good default.
	// @Range: 0 180
	// @Units: degrees/second
	// @Increment: 1
	// @User: Advanced
	AP_GROUPINFO("RMAX",   4, AP_RollController, _max_rate,           0),

	AP_GROUPEND
};

// Function returns an equivalent elevator deflection in centi-degrees in the range from -4500 to 4500
// A positive demand is up
// Inputs are: 
// 1) demanded bank angle in centi-degrees
// 2) control gain scaler = scaling_speed / aspeed
// 3) boolean which is true when stabilise mode is active
// 4) minimum FBW airspeed (metres/sec)
//
int32_t AP_RollController::get_servo_out(int32_t angle, float scaler, bool stabilize, int16_t aspd_min)
{
	uint32_t tnow = hal.scheduler->millis();
	uint32_t dt = tnow - _last_t;
	if (_last_t == 0 || dt > 1000) {
		dt = 0;
	}
	_last_t = tnow;
	
	// Calculate equivalent gains so that values for K_P and K_I can be taken across from the old PID law
    // No conversion is required for K_D
	float kp_ff = max((_K_P - _K_I * _tau) * _tau  - _K_D , 0);
	float ki_rate = _K_I * _tau;

	if(_ahrs == NULL) return 0; // can't control without a reference
	float delta_time    = (float)dt / 1000.0f;
	
	// Calculate bank angle error in centi-degrees
	int32_t angle_err = angle - _ahrs->roll_sensor;

	// Calculate the desired roll rate (deg/sec) from the angle error
	float desired_rate = angle_err * 0.01f / _tau;
	
	// Limit the demanded roll rate
	if (_max_rate && desired_rate < -_max_rate) desired_rate = -_max_rate;
	else if (_max_rate && desired_rate > _max_rate) desired_rate = _max_rate;
	
    // Get body rate vector (radians/sec)
	float omega_x = _ahrs->get_gyro().x;
	
	// Calculate the roll rate error (deg/sec) and apply gain scaler
	float rate_error = (desired_rate - ToDeg(omega_x)) * scaler;
	
	// Get an airspeed estimate - default to zero if none available
	float aspeed;
	if (!_ahrs->airspeed_estimate(&aspeed)) aspeed = 0.0f;

	// Multiply roll rate error by _ki_rate and integrate
	// Don't integrate if in stabilise mode as the integrator will wind up against the pilots inputs
	if (!stabilize) {
		//only integrate if gain and time step are positive and airspeed above min value.
		if ((ki_rate > 0) && (dt > 0) && (aspeed > float(aspd_min)))
		{
		    float integrator_delta = rate_error * ki_rate * scaler * delta_time;
			// prevent the integrator from increasing if surface defln demand is above the upper limit
			if (_last_out < -45) integrator_delta = max(integrator_delta , 0);
			// prevent the integrator from decreasing if surface defln demand  is below the lower limit
			else if (_last_out > 45) integrator_delta = min(integrator_delta , 0);
			_integrator += integrator_delta;
		}
	} else {
		_integrator = 0;
	}
	
	// Calculate the demanded control surface deflection
	// Note the scaler is applied again. We want a 1/speed scaler applied to the feed-forward
	// path, but want a 1/speed^2 scaler applied to the rate error path. 
	// This is because acceleration scales with speed^2, but rate scales with speed.
	_last_out = ( (rate_error * _K_D) + _integrator + (desired_rate * kp_ff) ) * scaler;
	
	// Convert to centi-degrees and constrain
	return constrain_float(_last_out * 100, -4500, 4500);
}

void AP_RollController::reset_I()
{
	_integrator = 0;
}

