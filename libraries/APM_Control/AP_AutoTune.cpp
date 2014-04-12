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

/**
   The strategy for roll/pitch autotune is to give the user a AUTOTUNE
   flight mode which behaves just like FBWA, but does automatic
   tuning.

   While the user is flying in AUTOTUNE the gains are saved every 10
   seconds, but the saved gains are not the current gains, instead it
   saves the gains from 10s ago. When the user exits AUTOTUNE the
   gains are restored from 10s ago.

   This allows the user to fly as much as they want in AUTOTUNE mode,
   and if they are ever unhappy they just exit the mode. If they stay
   in AUTOTUNE for more than 10s then their gains will have changed.

   Using this approach users don't need any special switches, they
   just need to be able to enter and exit AUTOTUNE mode
*/

#include <AP_HAL.h>
#include <AP_Common.h>
#include <AP_Math.h>
#include "AP_AutoTune.h"

extern const AP_HAL::HAL& hal;

// time in milliseconds between autotune saves
#define AUTOTUNE_SAVE_PERIOD 10000U

// how much time we have to overshoot for to reduce gains
#define AUTOTUNE_OVERSHOOT_TIME 100

// how much time we have to undershoot for to increase gains
#define AUTOTUNE_UNDERSHOOT_TIME 200

// step size for increasing gains
#define AUTOTUNE_INCREASE_STEP 0.05f

// step size for decreasing gains
#define AUTOTUNE_DECREASE_STEP 0.1f

// max rate
#define AUTOTUNE_MAX_RATE 60
#define AUTOTUNE_MIN_RATE 30

// min/max P gains
#define AUTOTUNE_MAX_P 4.0f
#define AUTOTUNE_MIN_P 0.3f

// constructor
AP_AutoTune::AP_AutoTune(ATGains &_gains) :
current(_gains) {}

#if CONFIG_HAL_BOARD == HAL_BOARD_AVR_SITL
#include <stdio.h>
 # define Debug(fmt, args ...)  do {hal.console->printf("%s:%d: " fmt "\n", __FUNCTION__, __LINE__, ## args); hal.scheduler->delay(1); } while(0)
#else
 # define Debug(fmt, args ...)
#endif

/*
  start an autotune session
*/
void AP_AutoTune::start(void)
{
    running = true;
    state = DEMAND_UNSATURATED;
    uint32_t now = hal.scheduler->millis();

    state_enter_ms = now;
    last_save_ms = now;
    
    restore = current;

    /*
      set some reasonable values for I and D if the user hasn't set
      them at all
     */
    if (current.I < 0.1f) {
        current.I.set(0.1f);
    }
    if (current.I > 0.5f) {
        current.I.set(0.5f);
    }
    if (current.imax < 2000) {
        current.imax.set(2000);
    }
    if (current.rmax <= 0 || current.rmax > AUTOTUNE_MAX_RATE) {
        current.rmax.set(AUTOTUNE_MAX_RATE);
    }
    if (current.rmax < AUTOTUNE_MIN_RATE) {
        current.rmax.set(AUTOTUNE_MIN_RATE);
    }
    if (current.D < 0.05f) {
        current.D.set(0.05f);
    }
    if (current.D > 0.5f) {
        current.D.set(0.5f);
    }

    next_save = current;

    Debug("START P -> %.3f\n", current.P.get());
}

/*
  called when we change state to see if we should change gains
 */
void AP_AutoTune::stop(void)
{
    running = false;
    save_gains(restore);
}


/*
  one update cycle of the autotuner
 */
void AP_AutoTune::update(float desired_rate, float achieved_rate, float servo_out)
{
    if (!running) {
        return;
    }
    check_save();

    // see what state we are in
    ATState new_state;
    float abs_desired_rate = fabsf(desired_rate);
    uint32_t now = hal.scheduler->millis();

    if (fabsf(servo_out) >= 4500) {
        // we have saturated the servo demand (not including
        // integrator), we cannot get any information that would allow
        // us to increase the gain
        saturated_surfaces = true;
    }

    if (abs_desired_rate < 0.8f * current.rmax) {
        // we are not demanding max rate
        new_state = DEMAND_UNSATURATED;
    } else if (fabsf(achieved_rate) > abs_desired_rate) {
        new_state = desired_rate > 0 ? DEMAND_OVER_POS : DEMAND_OVER_NEG;
    } else {
        new_state = desired_rate > 0 ? DEMAND_UNDER_POS : DEMAND_UNDER_NEG;
    }
    if (new_state != state) {
        check_state_exit(now - state_enter_ms);
        state = new_state;
        state_enter_ms = now;
        saturated_surfaces = false;
    }
}

/*
  called when we change state to see if we should change gains
 */
void AP_AutoTune::check_state_exit(uint32_t state_time_ms)
{
    switch (state) {
    case DEMAND_UNSATURATED:
        break;
    case DEMAND_UNDER_POS:
    case DEMAND_UNDER_NEG:
        // we increase P if we have not saturated the surfaces during
        // this state, and we have 
        if (state_time_ms >= AUTOTUNE_UNDERSHOOT_TIME && !saturated_surfaces) {
            current.P += AUTOTUNE_INCREASE_STEP;
            if (current.P > AUTOTUNE_MAX_P) {
                current.P = AUTOTUNE_MAX_P;
            }
            Debug("UNDER P -> %.3f\n", current.P.get());
        }
        break;
    case DEMAND_OVER_POS:
    case DEMAND_OVER_NEG:
        if (state_time_ms >= AUTOTUNE_OVERSHOOT_TIME) {
            current.P -= AUTOTUNE_DECREASE_STEP;
            if (current.P < AUTOTUNE_MIN_P) {
                current.P = AUTOTUNE_MIN_P;
            }
            Debug("OVER P -> %.3f\n", current.P.get());
        }
        break;        
    }
}

/*
  see if we should save new values
 */
void AP_AutoTune::check_save(void)
{
    if (hal.scheduler->millis() - last_save_ms < AUTOTUNE_SAVE_PERIOD) {
        return;
    }

    // save the next_save values, which are the autotune value from
    // the last save period. This means the pilot has
    // AUTOTUNE_SAVE_PERIOD milliseconds to decide they don't like the
    // gains and switch out of autotune
    ATGains tmp = current;

    save_gains(next_save);
    Debug("SAVE P -> %.3f\n", current.P.get());

    // restore our current gains
    current = tmp;

    // if the pilot exits autotune they get these saved values
    restore = next_save;

    // the next values to save will be the ones we are flying now
    next_save = current;
    last_save_ms = hal.scheduler->millis();
}

/*
  save a set of gains
 */
void AP_AutoTune::save_gains(const ATGains &v)
{
    current.tau.set_and_save_ifchanged(v.tau);
    current.P.set_and_save_ifchanged(v.P);
    current.I.set_and_save_ifchanged(v.I);
    current.D.set_and_save_ifchanged(v.D);
    current.rmax.set_and_save_ifchanged(v.rmax);
    current.imax.set_and_save_ifchanged(v.imax);
}
