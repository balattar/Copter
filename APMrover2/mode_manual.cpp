#include "mode.h"
#include "Rover.h"

void ModeManual::_exit()
{
    // clear lateral when exiting manual mode
    g2.motors.set_lateral(0);
}

void ModeManual::update()
{
    float desired_steering, desired_throttle, desired_lateral;
    get_pilot_desired_steering_and_throttle(desired_steering, desired_throttle);
    get_pilot_desired_lateral(desired_lateral);

    // if vehicle is balance bot, calculate actual throttle required for balancing
    if (rover.is_BalanceBot()) {
        rover.balance_pitch(desired_throttle, rover.arming.is_armed());
    }

    // copy RC scaled inputs to outputs
    g2.motors.set_throttle(desired_throttle);
    g2.motors.set_steering(desired_steering, false);
    g2.motors.set_lateral(desired_lateral);
}
