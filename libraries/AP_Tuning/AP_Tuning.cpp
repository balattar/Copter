// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "AP_Tuning.h"
#include <GCS_MAVLink/GCS.h>

extern const AP_HAL::HAL& hal;

const AP_Param::GroupInfo AP_Tuning::var_info[] = {
    // @Param: CHAN
    // @DisplayName: Transmitter tuning channel
    // @Description: This sets the channel for transmitter tuning. This should be connected to a knob or slider on your transmitter. It needs to be setup to use the PWM range given by TUNE_CHAN_MIN to TUNE_CHAN_MAX
    // @Values: 0:Disable,5:Chan5,6:Chan6,7:Chan7,8:Chan8,9:Chan9,10:Chan10,11:Chan11,12:Chan12,13:Chan13,14:Chan14,15:Chan15,16:Chan16
    // @User: Standard
    AP_GROUPINFO("CHAN", 1, AP_Tuning, channel, 0),
    
    // @Param: CHAN_MIN
    // @DisplayName: Transmitter tuning channel minimum pwm
    // @Description: This sets the PWM lower limit for the tuning channel
    // @Range: 900 2100
    // @User: Standard
    AP_GROUPINFO("CHAN_MIN", 2, AP_Tuning, channel_min, 1000),

    // @Param: CHAN_MAX
    // @DisplayName: Transmitter tuning channel maximum pwm
    // @Description: This sets the PWM upper limit for the tuning channel
    // @Range: 900 2100
    // @User: Standard
    AP_GROUPINFO("CHAN_MAX", 3, AP_Tuning, channel_max, 2000),

    // @Param: SELECTOR
    // @DisplayName: Transmitter tuning selector channel
    // @Description: This sets the channel for the transmitter tuning selector switch. This should be a 2 position switch, preferably spring loaded. A PWM above 1700 means high, below 1300 means low. If no selector is set then you won't be able to switch between parameters during flight or re-center the tuning knob
    // @Values: 0:Disable,1:Chan1,2:Chan3,3:Chan3,4:Chan4,5:Chan5,6:Chan6,7:Chan7,8:Chan8,9:Chan9,10:Chan10,11:Chan11,12:Chan12,13:Chan13,14:Chan14,15:Chan15,16:Chan16
    // @User: Standard
    AP_GROUPINFO("SELECTOR", 4, AP_Tuning, selector, 0),
    
    // @Param: RANGE
    // @DisplayName: Transmitter tuning range
    // @Description: This sets the range over which tuning will change a parameter. A value of 2 means the tuning parameter will go from 0.5 times the start value to 2x the start value over the range of the tuning channel
    // @User: Standard
    AP_GROUPINFO("RANGE", 5, AP_Tuning, range, 2.0f),

    AP_GROUPEND
};

/*
  handle selector switch input
*/
void AP_Tuning::check_selector_switch(void)
{
    if (selector == 0) {
        // no selector switch enabled
        return;
    }
    RC_Channel *selchan = RC_Channel::rc_channel(selector-1);
    if (selchan == nullptr) {
        return;
    }
    uint16_t selector_in = selchan->get_radio_in();
    if (selector_in >= 1700) {
        // high selector
        if (selector_start_ms == 0) {
            selector_start_ms = AP_HAL::millis();
        }
        uint32_t hold_time = AP_HAL::millis() - selector_start_ms;
        if (hold_time > 5000 && changed) {
            // save tune
            save_parameters();
            re_center();
            GCS_MAVLINK::send_statustext_all(MAV_SEVERITY_INFO, "Tuning: Saved");
            AP_Notify::events.tune_save = 1;
            changed = false;
            need_revert = 0;
        }
    } else if (selector_in <= 1300) {
        // low selector
        if (selector_start_ms != 0) {
            uint32_t hold_time = AP_HAL::millis() - selector_start_ms;
            if (hold_time < 2000) {
                // re-center the value
                re_center();
                GCS_MAVLINK::send_statustext_all(MAV_SEVERITY_INFO, "Tuning: recentered %s", get_tuning_name(current_parm));
            } else if (hold_time < 5000) {
                // change parameter
                next_parameter();
            }
        }
        selector_start_ms = 0;
    }
}

/*
  re-center the tuning value
 */
void AP_Tuning::re_center(void)
{
    AP_Float *f = get_param_pointer(current_parm);
    if (f != nullptr) {
        center_value = f->get();
    }
    mid_point_wait = true;
}

/*
  check for changed tuning input
 */
void AP_Tuning::check_input(uint8_t flightmode)
{
    if (channel <= 0 || parmset <= 0) {
        // disabled
        return;
    }

    // check for revert on changed flightmode
    if (flightmode != last_flightmode) {
        if (need_revert != 0) {
            GCS_MAVLINK::send_statustext_all(MAV_SEVERITY_INFO, "Tuning: reverted");
            revert_parameters();
            re_center();
        }
        last_flightmode = flightmode;
    }
    
    // only adjust values at 10Hz
    uint32_t now = AP_HAL::millis();
    uint32_t dt_ms = now - last_check_ms;
    if (dt_ms < 100) {
        return;
    }
    last_check_ms = now;

    if (channel > hal.rcin->num_channels()) {
        // not valid channel
        return;
    }

    // check for invalid range
    if (range < 1.1f) {
        range.set(1.1f);
    }

    if (current_parm == 0) {
        next_parameter();
    }

    // cope with user changing parmset while tuning
    if (current_set != parmset) {
        re_center();
    }
    current_set = parmset;
    
    check_selector_switch();

    if (selector_start_ms) {
        // no tuning while selector high
        return;
    }

    if (current_parm == 0) {
        return;
    }
    
    RC_Channel *chan = RC_Channel::rc_channel(channel-1);
    if (chan == nullptr) {
        return;
    }
    float chan_value = linear_interpolate(-1, 1, chan->get_radio_in(), channel_min, channel_max);
    if (dt_ms > 500) {
        last_channel_value = chan_value;
    }

    if (fabsf(chan_value - last_channel_value) < 0.01) {
        // ignore changes of less than 1%
        return;
    }

    //hal.console->printf("chan_value %.2f last_channel_value %.2f\n", chan_value, last_channel_value);

    if (mid_point_wait) {
        // see if we have crossed the mid-point. We use a small deadzone to make it easier
        // to move to the "indent" portion of a slider to start tuning
        const float dead_zone = 0.02;
        if ((chan_value > dead_zone && last_channel_value > 0) ||
            (chan_value < -dead_zone && last_channel_value < 0)) {
            // still waiting
            return;
        }
        // starting tuning
        mid_point_wait = false;
        GCS_MAVLINK::send_statustext_all(MAV_SEVERITY_INFO, "Tuning: mid-point %s", get_tuning_name(current_parm));
        AP_Notify::events.tune_started = 1;
    }
    last_channel_value = chan_value;

    float new_value;
    if (chan_value > 0) {
        new_value = linear_interpolate(center_value, range*center_value, chan_value, 0, 1);
    } else {
        new_value = linear_interpolate(center_value/range, center_value, chan_value, -1, 0);
    }
    changed = true;
    need_revert |= (1U << current_parm_index);
    set_value(current_parm, new_value);
    Log_Write_Parameter_Tuning(new_value);
}


/*
  log a tuning change
 */
void AP_Tuning::Log_Write_Parameter_Tuning(float value)
{
    DataFlash_Class::instance()->Log_Write("PTUN", "TimeUS,Set,Parm,Value,CenterValue", "QBBff",
                                           AP_HAL::micros64(),
                                           parmset,
                                           current_parm,
                                           value,
                                           center_value);
}

/*
  save parameters in the set
 */
void AP_Tuning::save_parameters(void)
{
    uint8_t set = (uint8_t)parmset.get();
    if (set < set_base) {
        // single parameter tuning
        save_value(set);
        return;
    }
    // multiple parameter tuning
    for (uint8_t i=0; tuning_sets[i].num_parms != 0; i++) {
        if (tuning_sets[i].set+set_base == set) {
            for (uint8_t p=0; p<tuning_sets[i].num_parms; p++) {
                save_value(tuning_sets[i].parms[p]);
            }
            break;
        }
    }
}


/*
  save parameters in the set
 */
void AP_Tuning::revert_parameters(void)
{
    uint8_t set = (uint8_t)parmset.get();
    if (set < set_base) {
        // single parameter tuning
        reload_value(set);
        return;
    }
    for (uint8_t i=0; tuning_sets[i].num_parms != 0; i++) {
        if (tuning_sets[i].set+set_base == set) {
            for (uint8_t p=0; p<tuning_sets[i].num_parms; p++) {
                if (p >= 32 || (need_revert & (1U<<p))) {
                    reload_value(tuning_sets[i].parms[p]);
                }
            }
            need_revert = 0;
            break;
        }
    }
}

/*
  switch to the next parameter in the set
 */
void AP_Tuning::next_parameter(void)
{
    uint8_t set = (uint8_t)parmset.get();
    if (set < set_base) {
        // nothing to do but re-center
        current_parm = set;
        re_center();        
        return;
    }
    for (uint8_t i=0; tuning_sets[i].num_parms != 0; i++) {
        if (tuning_sets[i].set+set_base == set) {
            if (current_parm == 0) {
                current_parm_index = 0;
            } else {
                current_parm_index = (current_parm_index + 1) % tuning_sets[i].num_parms;
            }
            current_parm = tuning_sets[i].parms[current_parm_index];
            re_center();
            GCS_MAVLINK::send_statustext_all(MAV_SEVERITY_INFO, "Tuning: started %s", get_tuning_name(current_parm));
            AP_Notify::events.tune_next = current_parm_index+1;
            break;
        }
    }
}

/*
  return a string representing a tuning parameter
 */
const char *AP_Tuning::get_tuning_name(uint8_t parm)
{
    for (uint8_t i=0; tuning_names[i].name != nullptr; i++) {
        if (parm == tuning_names[i].parm) {
            return tuning_names[i].name;
        }
    }
    return "UNKNOWN";
}

