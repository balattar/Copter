#include "mode.h"
#include "Rover.h"

void ModeSteering::update()
{
    // convert pilot throttle input to desired speed
    // speed in proportion to cruise speed, up to 50% throttle, then uses nudging above that.
    float target_speed = channel_throttle->get_control_in() * 0.01f * 2.0f * g.speed_cruise;
    target_speed = constrain_float(target_speed, -g.speed_cruise, g.speed_cruise);

    // in steering mode we control lateral acceleration directly. We first calculate the maximum lateral
    // acceleration at full steering lock for this speed. That is V^2/R where R is the radius of turn.
    // We get the radius of turn from half the STEER2SRV_P.
    const float ground_speed = rover.ground_speed;
    float max_g_force = ground_speed * ground_speed / MAX(g2.turn_radius, 0.1f);

    // constrain to user set TURN_MAX_G
    max_g_force = constrain_float(max_g_force, 0.1f, g.turn_max_g * GRAVITY_MSS);

    // convert pilot steering input to desired lateral acceleration
    lateral_acceleration = max_g_force * (channel_steer->get_control_in() / 4500.0f);

    // reverse target lateral acceleration if backing up
    bool reversed = false;
    if (is_negative(target_speed)) {
        reversed = true;
        lateral_acceleration = -lateral_acceleration;
        target_speed = fabsf(target_speed);
    }

    // mark us as in_reverse when using a negative throttle
    rover.set_reverse(reversed);

    // run steering controller
    calc_nav_steer(reversed);

    // run speed to throttle output controller
    calc_throttle(target_speed, reversed);
}
