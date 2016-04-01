// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 *       AP_MotorsSingle.cpp - ArduCopter motors library
 *       Code by RandyMackay. DIYDrones.com
 *
 */

#include <AP_HAL/AP_HAL.h>
#include <AP_Math/AP_Math.h>
#include "AP_MotorsCoax.h"

extern const AP_HAL::HAL& hal;


const AP_Param::GroupInfo AP_MotorsCoax::var_info[] = {
    // variables from parent vehicle
    AP_NESTEDGROUPINFO(AP_MotorsMulticopter, 0),

    // parameters 1 ~ 29 were reserved for tradheli
    // parameters 30 ~ 39 reserved for tricopter
    // parameters 40 ~ 49 for single copter and coax copter (these have identical parameter files)

    // @Param: ROLL_SV_REV
    // @DisplayName: Reverse roll feedback 
    // @Description: Ensure the feedback is negative
    // @Values: -1:Reversed,1:Normal
    AP_GROUPINFO("ROLL_SV_REV", 40, AP_MotorsCoax, _roll_reverse, AP_MOTORS_COAX_POSITIVE),

    // @Param: PITCH_SV_REV
    // @DisplayName: Reverse roll feedback 
    // @Description: Ensure the feedback is negative
    // @Values: -1:Reversed,1:Normal
    AP_GROUPINFO("PITCH_SV_REV", 41, AP_MotorsCoax, _pitch_reverse, AP_MOTORS_COAX_POSITIVE),

	// @Param: YAW_SV_REV
    // @DisplayName: Reverse roll feedback 
    // @Description: Ensure the feedback is negative
    // @Values: -1:Reversed,1:Normal
    AP_GROUPINFO("YAW_SV_REV", 42, AP_MotorsCoax, _yaw_reverse, AP_MOTORS_COAX_POSITIVE),

	// @Param: SV_SPEED
    // @DisplayName: Servo speed 
    // @Description: Servo update speed
    // @Values: -1:Opposite direction,1:Same direction
    AP_GROUPINFO("SV_SPEED", 43, AP_MotorsCoax, _servo_speed, AP_MOTORS_SINGLE_SPEED_DIGITAL_SERVOS),

    AP_GROUPEND
};
// init
void AP_MotorsCoax::Init()
{
    // set update rate for the 3 motors (but not the servo on channel 7)
    set_update_rate(_speed_hz);

    // set the motor_enabled flag so that the main ESC can be calibrated like other frame types
    motor_enabled[AP_MOTORS_MOT_5] = true;
    motor_enabled[AP_MOTORS_MOT_6] = true;

    // we set four servos to angle
    _servo1.set_type(RC_CHANNEL_TYPE_ANGLE);
    _servo2.set_type(RC_CHANNEL_TYPE_ANGLE);
    _servo3.set_type(RC_CHANNEL_TYPE_ANGLE);
    _servo4.set_type(RC_CHANNEL_TYPE_ANGLE);
    _servo1.set_angle(AP_MOTORS_COAX_SERVO_INPUT_RANGE);
    _servo2.set_angle(AP_MOTORS_COAX_SERVO_INPUT_RANGE);
    _servo3.set_angle(AP_MOTORS_COAX_SERVO_INPUT_RANGE);
    _servo4.set_angle(AP_MOTORS_COAX_SERVO_INPUT_RANGE);

    // disable CH7 from being used as an aux output (i.e. for camera gimbal, etc)
    RC_Channel_aux::disable_aux_channel(CH_7);
}

// set update rate to motors - a value in hertz
void AP_MotorsCoax::set_update_rate( uint16_t speed_hz )
{
    // record requested speed
    _speed_hz = speed_hz;

    // set update rate for the 4 servos and 2 motors
    uint32_t mask =
        1U << AP_MOTORS_MOT_1 |
        1U << AP_MOTORS_MOT_2 |
        1U << AP_MOTORS_MOT_3 |
        1U << AP_MOTORS_MOT_4 ;
    rc_set_freq(mask, _servo_speed);
    uint32_t mask2 =
        1U << AP_MOTORS_MOT_5 |
        1U << AP_MOTORS_MOT_6 ;
    rc_set_freq(mask2, _speed_hz);
}

// enable - starts allowing signals to be sent to motors
void AP_MotorsCoax::enable()
{
    // enable output channels
    rc_enable_ch(AP_MOTORS_MOT_1);
    rc_enable_ch(AP_MOTORS_MOT_2);
    rc_enable_ch(AP_MOTORS_MOT_3);
    rc_enable_ch(AP_MOTORS_MOT_4);
    rc_enable_ch(AP_MOTORS_MOT_5);
    rc_enable_ch(AP_MOTORS_MOT_6);
}

// output_min - sends minimum values out to the motor and trim values to the servos
void AP_MotorsCoax::output_min()
{
    // send minimum value to each motor
    hal.rcout->cork();
    rc_write(AP_MOTORS_MOT_1, _servo1.radio_trim);
    rc_write(AP_MOTORS_MOT_2, _servo2.radio_trim);
    rc_write(AP_MOTORS_MOT_3, _servo3.radio_trim);
    rc_write(AP_MOTORS_MOT_4, _servo4.radio_trim);
    rc_write(AP_MOTORS_MOT_5, _throttle_radio_min);
    rc_write(AP_MOTORS_MOT_6, _throttle_radio_min);
    hal.rcout->push();
}

void AP_MotorsCoax::output_to_motors()
{
    switch (_multicopter_flags.spool_mode) {
        case SHUT_DOWN:
            // sends minimum values out to the motors
            hal.rcout->cork();
            rc_write(AP_MOTORS_MOT_1, calc_pivot_radio_output(_roll_radio_passthrough*_servo_1_reverse, _servo_1_min, _servo_1_trim, _servo_1_max));
            rc_write(AP_MOTORS_MOT_2, calc_pivot_radio_output(_pitch_radio_passthrough *_servo_2_reverse, _servo_2_min, _servo_2_trim, _servo_2_max));
            rc_write(AP_MOTORS_MOT_3, calc_pivot_radio_output(_roll_radio_passthrough*_servo_3_reverse, _servo_3_min, _servo_3_trim, _servo_3_max));
            rc_write(AP_MOTORS_MOT_4, calc_pivot_radio_output(_pitch_radio_passthrough*_servo_4_reverse, _servo_4_min, _servo_4_trim, _servo_4_max));
            rc_write(AP_MOTORS_MOT_5, _throttle_radio_min);
            rc_write(AP_MOTORS_MOT_6, _throttle_radio_min);
            hal.rcout->push();
            break;
        case SPIN_WHEN_ARMED:
            // sends output to motors when armed but not flying
            hal.rcout->cork();
            rc_write(AP_MOTORS_MOT_1, calc_pivot_radio_output(_throttle_low_end_pct * _actuator_out[0]*_servo_1_reverse, _servo_1_min, _servo_1_trim, _servo_1_max));
            rc_write(AP_MOTORS_MOT_2, calc_pivot_radio_output(_throttle_low_end_pct * _actuator_out[1]*_servo_2_reverse, _servo_2_min, _servo_2_trim, _servo_2_max));
            rc_write(AP_MOTORS_MOT_3, calc_pivot_radio_output(_throttle_low_end_pct * _actuator_out[2]*_servo_3_reverse, _servo_3_min, _servo_3_trim, _servo_3_max));
            rc_write(AP_MOTORS_MOT_4, calc_pivot_radio_output(_throttle_low_end_pct * _actuator_out[3]*_servo_4_reverse, _servo_4_min, _servo_4_trim, _servo_4_max));
            rc_write(AP_MOTORS_MOT_5, constrain_int16(_throttle_radio_min + _throttle_low_end_pct * _min_throttle, _throttle_radio_min, _throttle_radio_min + _min_throttle));
            rc_write(AP_MOTORS_MOT_6, constrain_int16(_throttle_radio_min + _throttle_low_end_pct * _min_throttle, _throttle_radio_min, _throttle_radio_min + _min_throttle));
            hal.rcout->push();
            break;
        case SPOOL_UP:
        case THROTTLE_UNLIMITED:
        case SPOOL_DOWN:
            // set motor output based on thrust requests
            hal.rcout->cork();
            rc_write(AP_MOTORS_MOT_1, calc_pivot_radio_output(_actuator_out[0]*_servo_1_reverse, _servo_1_min, _servo_1_trim, _servo_1_max));
            rc_write(AP_MOTORS_MOT_2, calc_pivot_radio_output(_actuator_out[1]*_servo_2_reverse, _servo_2_min, _servo_2_trim, _servo_2_max));
            rc_write(AP_MOTORS_MOT_3, calc_pivot_radio_output(_actuator_out[2]*_servo_3_reverse, _servo_3_min, _servo_3_trim, _servo_3_max));
            rc_write(AP_MOTORS_MOT_4, calc_pivot_radio_output(_actuator_out[3]*_servo_4_reverse, _servo_4_min, _servo_4_trim, _servo_4_max));
            rc_write(AP_MOTORS_MOT_5, calc_thrust_to_pwm(_thrust_yt_ccw));
            rc_write(AP_MOTORS_MOT_6, calc_thrust_to_pwm(_thrust_yt_cw));
            hal.rcout->push();
            break;
    }
}

// get_motor_mask - returns a bitmask of which outputs are being used for motors or servos (1 means being used)
//  this can be used to ensure other pwm outputs (i.e. for servos) do not conflict
uint16_t AP_MotorsCoax::get_motor_mask()
{
    uint32_t mask =
        1U << AP_MOTORS_MOT_1 |
        1U << AP_MOTORS_MOT_2 |
        1U << AP_MOTORS_MOT_3 |
        1U << AP_MOTORS_MOT_4 |
        1U << AP_MOTORS_MOT_5 |
        1U << AP_MOTORS_MOT_6;
    return rc_map_mask(mask);
}

// sends commands to the motors
void AP_MotorsCoax::output_armed_stabilizing()
{
    float   roll_thrust;                // roll thrust input value, +/- 1.0
    float   pitch_thrust;               // pitch thrust input value, +/- 1.0
    float   yaw_thrust;                 // yaw thrust input value, +/- 1.0
    float   throttle_thrust;            // throttle thrust input value, 0.0 - 1.0
    float   thrust_min_rp;              // the minimum throttle setting that will not limit the roll and pitch output
    float   thr_adj;                    // the difference between the pilot's desired throttle and throttle_thrust_best_rpy
    float   throttle_thrust_hover = get_hover_throttle_as_high_end_pct();   // throttle hover thrust value, 0.0 - 1.0
    float   throttle_thrust_best_rpy;   // throttle providing maximum roll, pitch and yaw range without climbing
    float   throttle_thrust_rpy_mix;    // partial calculation of throttle_thrust_best_rpy
    float   y_scale;                    // this is used to scale the yaw to fit within the motor limits

    // apply voltage and air pressure compensation
    // todo: we shouldn't need input reversing with servo reversing
    roll_thrust = _roll_reverse * get_roll_thrust() * get_compensation_gain();
    pitch_thrust = _pitch_reverse * get_pitch_thrust() * get_compensation_gain();
    yaw_thrust = _yaw_reverse * get_yaw_thrust() * get_compensation_gain();
    throttle_thrust = get_throttle() * get_compensation_gain();

    // assuming maximum actuator defection the maximum roll and pitch torque is approximately proportional to thrust
    thrust_min_rp = MAX(fabsf(roll_thrust), fabsf(pitch_thrust));

    // sanity check throttle is above zero and below current limited throttle
    if (throttle_thrust <= 0.0f) {
        throttle_thrust = 0.0f;
        limit.throttle_lower = true;
    }
    if (throttle_thrust >= _throttle_thrust_max) {
        throttle_thrust = _throttle_thrust_max;
        limit.throttle_upper = true;
    }
    throttle_thrust_rpy_mix = MAX(throttle_thrust, throttle_thrust*MAX(0.0f,1.0f-_throttle_rpy_mix)+throttle_thrust_hover*_throttle_rpy_mix);

    // check everything fits
    throttle_thrust_best_rpy = MIN(0.5f, throttle_thrust_rpy_mix);
    if (is_zero(yaw_thrust)) {
        y_scale = 1.0f;
    } else {
        y_scale = constrain_float(throttle_thrust_best_rpy/fabsf(0.5f * yaw_thrust), 0.0f, 1.0f);
    }

    thr_adj = throttle_thrust - throttle_thrust_best_rpy;
    if(y_scale < 1.0f){
        // Full range is being used yaw.
        limit.yaw = true;
        if(thr_adj < 0.0f){
            limit.throttle_lower = true;
        }else if(thr_adj > 0.0f){
            limit.throttle_upper = true;
        }
        thr_adj = 0.0f;
    }else{
        if(thr_adj < MIN(-(throttle_thrust_best_rpy - fabsf(0.5f * yaw_thrust)), -(throttle_thrust_best_rpy - thrust_min_rp))){
            // Throttle can't be reduced to the desired level for one of two reasons:
            // 1. This would result in yaw control deviation causing the throttle output to be out of range.
            // 2. This would roll or pitch control would not be able to reach the desired level because of lack of thrust.
            thr_adj = MIN(-(throttle_thrust_best_rpy - fabsf(0.5f * yaw_thrust)), -(throttle_thrust_best_rpy - thrust_min_rp));
            limit.throttle_lower = true;
            if(thrust_min_rp > throttle_thrust_best_rpy + thr_adj){
                // todo: add limits for roll and pitch separately
                limit.roll_pitch = true;
            }
        }else if(thr_adj > 1.0f - (throttle_thrust_best_rpy + fabsf(0.5f * yaw_thrust))){
            // Throttle can't be increased to desired value
            thr_adj = 1.0f - (throttle_thrust_best_rpy + fabsf(0.5f * yaw_thrust));
            limit.throttle_upper = true;
        }
    }

    _thrust_yt_ccw = throttle_thrust_best_rpy + thr_adj + 0.5f * y_scale *_thrust_yt_ccw;
    _thrust_yt_cw = throttle_thrust_best_rpy + thr_adj - 0.5f * y_scale *_thrust_yt_cw;

    if(is_zero((throttle_thrust_best_rpy + thr_adj))){
        limit.roll_pitch = true;
        if(roll_thrust < 0.0f){
            _actuator_out[0] = -1.0f;
        }else if(roll_thrust > 0.0f){
            _actuator_out[0] = 1.0f;
        }else{
            _actuator_out[0] = 0.0f;
        }
        if(roll_thrust < 0.0f){
            _actuator_out[1] = -1.0f;
        }else if(roll_thrust > 0.0f){
            _actuator_out[1] = 1.0f;
        }else{
            _actuator_out[1] = 0.0f;
        }
    }else{
        // force of a lifting surface is approximately equal to the angle of attack times the airflow velocity squared
        // static thrust is proportional to the airflow velocity squared
        // therefore the torque of the roll and pitch actuators should be approximately proportional to
        // the angle of attack multiplied by the static thrust.
        _actuator_out[0] = roll_thrust/(throttle_thrust_best_rpy + thr_adj);
        _actuator_out[1] = pitch_thrust/(throttle_thrust_best_rpy + thr_adj);
        if(fabsf(_actuator_out[0]) > 1.0f){
            limit.roll_pitch = true;
            _actuator_out[0] = constrain_float(_actuator_out[0], -1.0f, 1.0f);
        }
        if(fabsf(_actuator_out[1]) > 1.0f){
            limit.roll_pitch = true;
            _actuator_out[1] = constrain_float(_actuator_out[1], -1.0f, 1.0f);
        }
    }
    _actuator_out[2] = _actuator_out[0];
    _actuator_out[3] = _actuator_out[1];
}

// output_test - spin a motor at the pwm value specified
//  motor_seq is the motor's sequence number from 1 to the number of motors on the frame
//  pwm value is an actual pwm value that will be output, normally in the range of 1000 ~ 2000
void AP_MotorsCoax::output_test(uint8_t motor_seq, int16_t pwm)
{
    // exit immediately if not armed
    if (!armed()) {
        return;
    }

    // output to motors and servos
    switch (motor_seq) {
        case 1:
            // flap servo 1
            rc_write(AP_MOTORS_MOT_1, pwm);
            break;
        case 2:
            // flap servo 2
            rc_write(AP_MOTORS_MOT_2, pwm);
            break;
        case 3:
            // flap servo 3
            rc_write(AP_MOTORS_MOT_3, pwm);
            break;
        case 4:
            // flap servo 4
            rc_write(AP_MOTORS_MOT_4, pwm);
            break;
        case 5:
            // motor 1
            rc_write(AP_MOTORS_MOT_5, pwm);
            break;
        case 6:
            // motor 2
            rc_write(AP_MOTORS_MOT_6, pwm);
            break;
        default:
            // do nothing
            break;
    }
}

// calc_yaw_radio_output - calculate final radio output for yaw channel
int16_t AP_MotorsCoax::calc_pivot_radio_output(float yaw_input, int16_t servo_min, int16_t servo_trim, int16_t servo_max)
{
    int16_t ret;

    if (yaw_input >= 0){
        ret = ((yaw_input * (servo_max - servo_trim)) + servo_trim);
    } else {
        ret = ((yaw_input * (servo_trim - servo_min)) + servo_trim);
    }

    return ret;
}
