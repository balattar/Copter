/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
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
  helicopter simulator class
*/

#include <AP_HAL/AP_HAL.h>
#include "SIM_Helicopter.h"
#include <stdio.h>

namespace SITL {

Helicopter::Helicopter(const char *home_str, const char *frame_str) :
    Aircraft(home_str, frame_str)
{
    mass = 2.13f;

    /*
       scaling from motor power to Newtons. Allows the copter
       to hover against gravity when the motor is at hover_throttle
    */
    thrust_scale = (mass * GRAVITY_MSS) / hover_throttle;

    // calculate lateral thrust ratio for tail rotor
    tail_thrust_scale = sinf(radians(hover_lean)) * thrust_scale / yaw_zero;

    frame_height = 0.1;

    if (strstr(frame_str, "-dual")) {
        frame_type = HELI_FRAME_DUAL;
    } else if (strstr(frame_str, "-compound")) {
        frame_type = HELI_FRAME_COMPOUND;
    } else {
        frame_type = HELI_FRAME_CONVENTIONAL;
    }
    gas_heli = (strstr(frame_str, "-gas") != NULL);
}

/*
  update the helicopter simulation by one time step
 */
void Helicopter::update(const struct sitl_input &input)
{
    // how much time has passed?
    float delta_time = frame_time_us * 1.0e-6f;

    float rsc = (input.servos[7]-1000) / 1000.0f;
    // ignition only for gas helis
    bool ignition_enabled = gas_heli?(input.servos[5] > 1500):true;

    float thrust = 0;
    float roll_rate = 0;
    float pitch_rate = 0;
    float yaw_rate = 0;
    float torque_effect_accel = 0;
    float lateral_x_thrust = 0;
    float lateral_y_thrust = 0;

    float swash1 = (input.servos[0]-1000) / 1000.0f;
    float swash2 = (input.servos[1]-1000) / 1000.0f;
    float swash3 = (input.servos[2]-1000) / 1000.0f;

    if (!ignition_enabled) {
        rsc = 0;
    }
    float rsc_scale = rsc/rsc_setpoint;

    switch (frame_type) {
    case HELI_FRAME_CONVENTIONAL: {
        // simulate a traditional helicopter

        float tail_rotor = (input.servos[3]-1000) / 1000.0f;

        thrust = (rsc/rsc_setpoint) * (swash1+swash2+swash3) / 3.0f;
        torque_effect_accel = (rsc_scale+thrust) * rotor_rot_accel;

        roll_rate = swash1 - swash2;
        pitch_rate = (swash1+swash2) / 2.0f - swash3;
        yaw_rate = tail_rotor - 0.5f;

        lateral_y_thrust = yaw_rate * rsc_scale * tail_thrust_scale;
        break;
    }

    case HELI_FRAME_DUAL: {
        // simulate a tandem helicopter

        float swash4 = (input.servos[3]-1000) / 1000.0f;
        float swash5 = (input.servos[4]-1000) / 1000.0f;
        float swash6 = (input.servos[5]-1000) / 1000.0f;

        thrust = (rsc / rsc_setpoint) * (swash1+swash2+swash3+swash4+swash5+swash6) / 6.0f;
        torque_effect_accel = (rsc_scale + rsc / rsc_setpoint) * rotor_rot_accel * ((swash1+swash2+swash3) - (swash4+swash5+swash6));

        roll_rate = (swash1-swash2) + (swash4-swash5);
        pitch_rate = (swash1+swash2+swash3) - (swash4+swash5+swash6);
        yaw_rate = (swash1-swash2) + (swash5-swash4);
        break;
    }

    case HELI_FRAME_COMPOUND: {
        // simulate a compound helicopter

        float right_rotor = (input.servos[3]-1000) / 1000.0f;
        float left_rotor = (input.servos[4]-1000) / 1000.0f;

        thrust = (rsc/rsc_setpoint) * (swash1+swash2+swash3) / 3.0f;
        torque_effect_accel = (rsc_scale+thrust) * rotor_rot_accel;

        roll_rate = swash1 - swash2;
        pitch_rate = (swash1+swash2) / 2.0f - swash3;
        yaw_rate = right_rotor - left_rotor;

        lateral_x_thrust = (left_rotor+right_rotor-1) * rsc_scale * tail_thrust_scale;
        break;
    }
    }

    roll_rate *= rsc_scale;
    pitch_rate *= rsc_scale;
    yaw_rate *= rsc_scale;

    // rotational acceleration, in rad/s/s, in body frame
    Vector3f rot_accel;
    rot_accel.x = roll_rate * roll_rate_max;
    rot_accel.y = pitch_rate * pitch_rate_max;
    rot_accel.z = yaw_rate * yaw_rate_max;

    // rotational air resistance
    rot_accel.x -= gyro.x * radians(5000.0) / terminal_rotation_rate;
    rot_accel.y -= gyro.y * radians(5000.0) / terminal_rotation_rate;
    rot_accel.z -= gyro.z * radians(400.0)  / terminal_rotation_rate;

    // torque effect on tail
    rot_accel.z += torque_effect_accel;

    // update rotational rates in body frame
    gyro += rot_accel * delta_time;

    // update attitude
    dcm.rotate(gyro * delta_time);
    dcm.normalize();

    // air resistance
    Vector3f air_resistance = -velocity_ef * (GRAVITY_MSS/terminal_velocity);

    // scale thrust to newtons
    thrust *= thrust_scale;

    accel_body = Vector3f(lateral_x_thrust, lateral_y_thrust, -thrust / mass);
    Vector3f accel_earth = dcm * accel_body;
    accel_earth += Vector3f(0, 0, GRAVITY_MSS);
    accel_earth += air_resistance;

    // if we're on the ground, then our vertical acceleration is limited
    // to zero. This effectively adds the force of the ground on the aircraft
    if (on_ground(position) && accel_earth.z > 0) {
        accel_earth.z = 0;
    }

    // work out acceleration as seen by the accelerometers. It sees the kinematic
    // acceleration (ie. real movement), plus gravity
    accel_body = dcm.transposed() * (accel_earth + Vector3f(0, 0, -GRAVITY_MSS));

    // add some noise
    add_noise(thrust / thrust_scale);

    // new velocity vector
    velocity_ef += accel_earth * delta_time;

    // new position vector
    Vector3f old_position = position;
    position += velocity_ef * delta_time;

    // assume zero wind for now
    airspeed = velocity_ef.length();

    // constrain height to the ground
    if (on_ground(position)) {
        if (!on_ground(old_position)) {
            printf("Hit ground at %f m/s\n", velocity_ef.z);

            velocity_ef.zero();

            // zero roll/pitch, but keep yaw
            float r, p, y;
            dcm.to_euler(&r, &p, &y);
            dcm.from_euler(0, 0, y);

            position.z = -(ground_level + frame_height - home.alt*0.01f);
        }
    }

    // update lat/lon/altitude
    update_position();
}

} // namespace SITL
