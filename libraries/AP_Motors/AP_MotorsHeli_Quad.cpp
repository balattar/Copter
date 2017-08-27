/*
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <AP_HAL/AP_HAL.h>

#include "AP_MotorsHeli_Quad.h"

extern const AP_HAL::HAL& hal;

const AP_Param::GroupInfo AP_MotorsHeli_Quad::var_info[] = {
    AP_NESTEDGROUPINFO(AP_MotorsHeli, 0),

    // @Param: RSC_PWM_MIN
    // @DisplayName: RSC PWM output miniumum
    // @Description: This sets the PWM output on RSC channel for maximum rotor speed
    // @Range: 0 2000
    // @User: Standard
    AP_GROUPINFO("RSC_PWM_MIN", 1, AP_MotorsHeli_Quad, _rotor._pwm_min, 1000),

    // @Param: RSC_PWM_MAX
    // @DisplayName: RSC PWM output maxiumum
    // @Description: This sets the PWM output on RSC channel for miniumum rotor speed
    // @Range: 0 2000
    // @User: Standard
    AP_GROUPINFO("RSC_PWM_MAX", 2, AP_MotorsHeli_Quad, _rotor._pwm_max, 2000),

    // @Param: RSC_PWM_REV
    // @DisplayName: RSC PWM reversal
    // @Description: This controls reversal of the RSC channel output
    // @Values: -1:Reversed,1:Normal
    // @User: Standard
    AP_GROUPINFO("RSC_PWM_REV", 3, AP_MotorsHeli_Quad, _rotor._pwm_rev, 1),

    AP_GROUPEND
};

// set update rate to motors - a value in hertz
void AP_MotorsHeli_Quad::set_update_rate( uint16_t speed_hz )
{
    // record requested speed
    _speed_hz = speed_hz;

    // setup fast channels
    uint32_t mask = 0;
    for (uint8_t i=0; i<AP_MOTORS_HELI_QUAD_NUM_MOTORS; i++) {
        mask |= 1U << (AP_MOTORS_MOT_1+i);
    }

    rc_set_freq(mask, _speed_hz);
}

// enable - starts allowing signals to be sent to motors
void AP_MotorsHeli_Quad::enable()
{
    // enable output channels
    for (uint8_t i=0; i<AP_MOTORS_HELI_QUAD_NUM_MOTORS; i++) {
        rc_enable_ch(AP_MOTORS_MOT_1+i);
    }

    rc_enable_ch(AP_MOTORS_HELI_QUAD_RSC);
}

// init_outputs
bool AP_MotorsHeli_Quad::init_outputs()
{
    if (_flags.initialised_ok) {
        return true;
    }

    for (uint8_t i=0; i<AP_MOTORS_HELI_QUAD_NUM_MOTORS; i++) {
        rc_enable_ch(AP_MOTORS_MOT_1+i);
        _servo[i] = SRV_Channels::get_channel_for(SRV_Channel::Aux_servo_function_t(SRV_Channel::k_motor1+i), CH_1+i);
        if (!_servo[i]) {
            return false;
        }
    }
    
    // set rotor servo range
    _rotor.init_servo();

    _flags.initialised_ok = true;

    return true;
}

// output_test - spin a motor at the pwm value specified
//  motor_seq is the motor's sequence number from 1 to the number of motors on the frame
//  pwm value is an actual pwm value that will be output, normally in the range of 1000 ~ 2000
void AP_MotorsHeli_Quad::output_test(uint8_t motor_seq, int16_t pwm)
{
    // exit immediately if not armed
    if (!armed()) {
        return;
    }

    // output to motors and servos
    switch (motor_seq) {
    case 1 ... AP_MOTORS_HELI_QUAD_NUM_MOTORS:
        rc_write(AP_MOTORS_MOT_1 + (motor_seq-1), pwm);
        break;
    case AP_MOTORS_HELI_QUAD_NUM_MOTORS+1:
        // main rotor
        rc_write(AP_MOTORS_HELI_QUAD_RSC, pwm);
        break;
    default:
        // do nothing
        break;
    }
}

// set_desired_rotor_speed
void AP_MotorsHeli_Quad::set_desired_rotor_speed(float desired_speed)
{
    _rotor.set_desired_speed(desired_speed);
}

// calculate_armed_scalars
void AP_MotorsHeli_Quad::calculate_armed_scalars()
{
    _rotor.set_ramp_time(_rsc_ramp_time);
    _rotor.set_runup_time(_rsc_runup_time);
    _rotor.set_critical_speed(_rsc_critical/1000.0f);
    _rotor.set_idle_output(_rsc_idle_output/1000.0f);
    _rotor.set_power_output_range(_rsc_power_low/1000.0f, _rsc_power_high/1000.0f, _rsc_power_high/1000.0f, 0);
}

// calculate_scalars
void AP_MotorsHeli_Quad::calculate_scalars()
{
    // range check collective min, max and mid
    if( _collective_min >= _collective_max ) {
        _collective_min = AP_MOTORS_HELI_COLLECTIVE_MIN;
        _collective_max = AP_MOTORS_HELI_COLLECTIVE_MAX;
    }

    _collective_mid = constrain_int16(_collective_mid, _collective_min, _collective_max);

    // calculate collective mid point as a number from 0 to 1000
    _collective_mid_pct = ((float)(_collective_mid-_collective_min))/((float)(_collective_max-_collective_min));

    // calculate factors based on swash type and servo position
    calculate_roll_pitch_collective_factors();

    // set mode of main rotor controller and trigger recalculation of scalars
    _rotor.set_control_mode(static_cast<RotorControlMode>(_rsc_mode.get()));
    calculate_armed_scalars();
}

// calculate_swash_factors - calculate factors based on swash type and servo position
void AP_MotorsHeli_Quad::calculate_roll_pitch_collective_factors()
{
    // assume X quad layout, with motors at 45, 135, 225 and 315 degrees
    // order FrontRight, RearLeft, FrontLeft, RearLeft
    const float angles[AP_MOTORS_HELI_QUAD_NUM_MOTORS] = { 45, 225, 315, 135 };
    const bool clockwise[AP_MOTORS_HELI_QUAD_NUM_MOTORS] = { false, false, true, true };
    const float cos45 = cosf(radians(45));
    
    for (uint8_t i=0; i<AP_MOTORS_HELI_QUAD_NUM_MOTORS; i++) {
        _rollFactor[CH_1+i]       = -0.5*sinf(radians(angles[i]))/cos45;
        _pitchFactor[CH_1+i]      =  0.5*cosf(radians(angles[i]))/cos45;
        _yawFactor[CH_1+i]        = clockwise[i]?-0.5:0.5;
        _collectiveFactor[CH_1+i] = 1;
    }
}

// get_motor_mask - returns a bitmask of which outputs are being used for motors or servos (1 means being used)
//  this can be used to ensure other pwm outputs (i.e. for servos) do not conflict
uint16_t AP_MotorsHeli_Quad::get_motor_mask()
{
    uint16_t mask = 0;
    for (uint8_t i=0; i<AP_MOTORS_HELI_QUAD_NUM_MOTORS; i++) {
        mask |= 1U << (AP_MOTORS_MOT_1+i);
    }
    mask |= 1U << AP_MOTORS_HELI_QUAD_RSC;
    return mask;
}

// update_motor_controls - sends commands to motor controllers
void AP_MotorsHeli_Quad::update_motor_control(RotorControlState state)
{
    // Send state update to motors
    _rotor.output(state);

    if (state == ROTOR_CONTROL_STOP) {
        // set engine run enable aux output to not run position to kill engine when disarmed
        SRV_Channels::set_output_limit(SRV_Channel::k_engine_run_enable, SRV_Channel::SRV_CHANNEL_LIMIT_MIN);
    } else {
        // else if armed, set engine run enable output to run position
        SRV_Channels::set_output_limit(SRV_Channel::k_engine_run_enable, SRV_Channel::SRV_CHANNEL_LIMIT_MAX);
    }

    // Check if rotors are run-up
    _heliflags.rotor_runup_complete = _rotor.is_runup_complete();
}

//
// move_actuators - moves swash plate to attitude of parameters passed in
//                - expected ranges:
//                       roll : -1 ~ +1
//                       pitch: -1 ~ +1
//                       collective: 0 ~ 1
//                       yaw:   -1 ~ +1
//
void AP_MotorsHeli_Quad::move_actuators(float roll_out, float pitch_out, float collective_in, float yaw_out)
{
    // initialize limits flag
    limit.roll_pitch = false;
    limit.yaw = false;
    limit.throttle_lower = false;
    limit.throttle_upper = false;

    // constrain collective input
    float collective_out = collective_in;
    if (collective_out <= 0.0f) {
        collective_out = 0.0f;
        limit.throttle_lower = true;
    }
    if (collective_out >= 1.0f) {
        collective_out = 1.0f;
        limit.throttle_upper = true;
    }

    // ensure not below landed/landing collective
    if (_heliflags.landing_collective && collective_out < (_land_collective_min/1000.0f)) {
        collective_out = _land_collective_min/1000.0f;
        limit.throttle_lower = true;
    }

    if (_heliflags.inverted_flight) {
        collective_out = 1 - collective_out;
    }
    
    // feed power estimate into main rotor controller
    _rotor.set_motor_load(fabsf(collective_out - _collective_mid_pct));

    // scale collective to -1 to 1
    collective_out = collective_out*2-1;

    if (collective_out < 0) {
        // with negative collective yaw torque is reversed
        yaw_out = -yaw_out;
    }

    float out[AP_MOTORS_HELI_QUAD_NUM_MOTORS] {};
    for (uint8_t i=0; i<AP_MOTORS_HELI_QUAD_NUM_MOTORS; i++) {
        out[i] =
            _rollFactor[CH_1+i] * roll_out +
            _pitchFactor[CH_1+i] * pitch_out +
            _yawFactor[CH_1+i] * yaw_out +
            _collectiveFactor[CH_1+i] * collective_out;
    }
    
    // move the servos
    for (uint8_t i=0; i<AP_MOTORS_HELI_QUAD_NUM_MOTORS; i++) {
        rc_write(AP_MOTORS_MOT_1+i, calc_pwm_output_1to1(out[i], _servo[i]));
    }
}


// servo_test - move servos through full range of movement
void AP_MotorsHeli_Quad::servo_test()
{
    // not implemented
}
