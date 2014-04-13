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

// step size for increasing gains, percentage
#define AUTOTUNE_INCREASE_STEP 5

// step size for decreasing gains, percentage
#define AUTOTUNE_DECREASE_STEP 8

// min/max P gains
#define AUTOTUNE_MAX_P 5.0f
#define AUTOTUNE_MIN_P 0.3f

// tau ranges
#define AUTOTUNE_MAX_TAU 0.7
#define AUTOTUNE_MIN_TAU 0.2

#define AUTOTUNE_MIN_IMAX 2000
#define AUTOTUNE_MAX_IMAX 4000

// constructor
AP_AutoTune::AP_AutoTune(ATGains &_gains, ATType _type, 
                         const AP_Vehicle::FixedWing &parms, 
                         DataFlash_Class &_dataflash) :
    current(_gains),
    type(_type),
    aparm(parms),
    dataflash(_dataflash)
{}

#if CONFIG_HAL_BOARD == HAL_BOARD_AVR_SITL
#include <stdio.h>
# define Debug(fmt, args ...)  do {::printf("%s:%d: " fmt "\n", __FUNCTION__, __LINE__, ## args); } while(0)
#else
 # define Debug(fmt, args ...)
#endif

/*
  auto-tuning table. This table gives the starting values for key
  tuning parameters based on a user chosen AUTOTUNE_LEVEL parameter
  from 1 to 10. Level 1 is a very soft tune. Level 10 is a very
  agressive tune.
 */
static const struct {
    float tau;
    float D;
    float rmax;
} tuning_table[] PROGMEM = {
    { 0.70f, 0.02f,  20 },   // level 1
    { 0.65f, 0.03f,  30 },   // level 2
    { 0.60f, 0.04f,  40 },   // level 3
    { 0.55f, 0.05f,  50 },   // level 4
    { 0.50f, 0.05f,  60 },   // level 5
    { 0.45f, 0.07f,  75 },   // level 6
    { 0.40f, 0.10f,  90 },   // level 7
    { 0.30f, 0.15f, 120 },   // level 8
    { 0.20f, 0.20f, 160 },   // level 9
    { 0.10f, 0.30f, 210 },   // level 10
    { 0.05f, 0.60f, 300 },   // (yes, it goes to 11)
};

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
    
    last_save = current;
    restore = current;

    uint8_t level = aparm.autotune_level;
    if (level > sizeof(tuning_table)/sizeof(tuning_table[0])) {
        level = sizeof(tuning_table)/sizeof(tuning_table[0]);
    }
    if (level < 1) {
        level = 1;
    }

    current.rmax.set(pgm_read_float(&tuning_table[level-1].rmax));
    current.D.set(   pgm_read_float(&tuning_table[level-1].D));
    current.tau.set( pgm_read_float(&tuning_table[level-1].tau));

    current.imax = constrain_float(current.imax, AUTOTUNE_MIN_IMAX, AUTOTUNE_MAX_IMAX);

    // force a 45 degree phase
    current.I = current.D / current.tau;

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

    if (fabsf(servo_out) >= 45) {
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
    if (state != DEMAND_UNSATURATED) {
        write_log(servo_out, desired_rate, achieved_rate);
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
            current.P.set(current.P * (100+AUTOTUNE_INCREASE_STEP) * 0.01f);
            if (current.P > AUTOTUNE_MAX_P) {
                current.P = AUTOTUNE_MAX_P;
            }
            Debug("UNDER P -> %.3f\n", current.P.get());
        }
        break;
    case DEMAND_OVER_POS:
    case DEMAND_OVER_NEG:
        if (state_time_ms >= AUTOTUNE_OVERSHOOT_TIME) {
            current.P.set(current.P * (100-AUTOTUNE_DECREASE_STEP) * 0.01f);
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
    current = last_save;
    current.tau.set_and_save_ifchanged(v.tau);
    current.P.set_and_save_ifchanged(v.P);
    current.I.set_and_save_ifchanged(v.I);
    current.D.set_and_save_ifchanged(v.D);
    current.rmax.set_and_save_ifchanged(v.rmax);
    current.imax.set_and_save_ifchanged(v.imax);
    last_save = current;
}

#define LOG_MSG_ATRP 211

struct PACKED log_ATRP {
    LOG_PACKET_HEADER;
    uint32_t timestamp;
    uint8_t  type;
    uint8_t  state;
    int16_t  servo;
    float    demanded;
    float    achieved;
    float    P;
};

static const struct LogStructure at_log_structures[] PROGMEM = {
    { LOG_MSG_ATRP, sizeof(log_ATRP),
      "ATRP", "IBBcfff",  "TimeMS,Type,State,Servo,Demanded,Achieved,P" },
};

void AP_AutoTune::write_log_headers(void)
{
    if (!logging_started) {
        logging_started = true;
        dataflash.AddLogFormats(at_log_structures, 1);
    }
}

void AP_AutoTune::write_log(float servo, float demanded, float achieved)
{
    if (!dataflash.logging_started()) {
        return;
    }

    write_log_headers();

    struct log_ATRP pkt = {
        LOG_PACKET_HEADER_INIT(LOG_MSG_ATRP),
        timestamp  : hal.scheduler->millis(),
        type       : type,
    	state      : (uint8_t)state,
        servo      : (int16_t)(servo*100),
        demanded   : demanded,
        achieved   : achieved,
        P          : current.P.get()
    };
    dataflash.WriteBlock(&pkt, sizeof(pkt));    
}
