/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

/*
 * control_hybrid.pde - init and run calls for hybrid flight mode
 *     hybrid tries to improve upon regular loiter by mixing the pilot input with the loiter controller
 */

#define HYBRID_SPEED_0                          10      // speed below which it is always safe to switch to loiter
#define HYBRID_LOITER_STAB_TIMER                300     // Must be higher than HYBRID_BRAKE_LOITER_MIX_TIMER (twice is a good deal) set it from 100 to 500, the number of centiseconds between loiter engage and getting wind_comp (once loiter stabilized)
#define HYBRID_BRAKE_TIME_ESTIMATE_MAX          600     // max number of cycles the brake will be applied before we switch to loiter
#define HYBRID_BRAKE_TO_LOITER_TIMER            150     // Number of cycles to transition from brake mode to loiter mode.  Must be lower than HYBRID_LOITER_STAB_TIMER
#define HYBRID_LOITER_TO_PILOT_MIX_TIMER        50      // Set it from 100 to 200, the number of centiseconds loiter and manual commands are mixed to make a smooth transition.
#define HYBRID_SMOOTH_RATE_FACTOR               0.04f   // filter applied to pilot's roll/pitch input as it returns to center.  A lower number will cause the roll/pitch to return to zero more slowly if the brake_rate is also low.
#define HYBRID_STICK_RELEASE_SMOOTH_ANGLE       1800    // max angle required (in centi-degrees) after which the smooth stick release effect is applied
#define HYBRID_WIND_COMP_ESTIMATE_SPEED_MAX     30      // wind compensation estimates will only run when velocity is at or below this speed in cm/s

// declare some function to keep compiler happy
static void hybrid_update_pilot_lean_angle(int16_t &lean_angle_filtered, int16_t &lean_angle_raw);
static int16_t hybrid_mix_controls(float mix_ratio, int16_t first_control, int16_t second_control);
static void hybrid_update_brake_angle_from_velocity(int16_t &brake_angle, float velocity);
static void hybrid_update_wind_comp_estimate();
static void hybrid_get_wind_comp_lean_angles(int16_t &roll_angle, int16_t &pitch_angle);

// mission state enumeration
enum hybrid_rp_mode {
    HYBRID_PILOT_OVERRIDE=0,            // pilot is controlling this axis (i.e. roll or pitch)
    HYBRID_BRAKE,                       // this axis is braking towards zero
    HYBRID_BRAKE_READY_TO_LOITER,       // this axis has completed braking and is ready to enter loiter mode (both modes must be this value before moving to next stage)
    HYBRID_BRAKE_TO_LOITER,             // both vehicle's axis (roll and pitch) are transitioning from braking to loiter mode (braking and loiter controls are mixed)
    HYBRID_LOITER,                      // both vehicle axis are holding position
    HYBRID_LOITER_TO_PILOT_OVERRIDE     // pilot has input controls on this axis and this axis is transitioning to pilot override (other axis will transition to brake if no pilot input)
};

static struct {
    hybrid_rp_mode roll_mode            : 3;    // roll mode: pilot override, brake or loiter
    hybrid_rp_mode pitch_mode           : 3;    // pitch mode: pilot override, brake or loiter
    uint8_t braking_time_updated_roll   : 1;    // true once we have re-estimated the braking time.  This is done once as the vehicle begins to flatten out after braking
    uint8_t braking_time_updated_pitch  : 1;    // true once we have re-estimated the braking time.  This is done once as the vehicle begins to flatten out after braking

    // pilot input related variables
    int16_t pilot_roll;                         // pilot requested roll angle (filtered to slow returns to zero)
    int16_t pilot_pitch;                        // pilot requested roll angle (filtered to slow returns to zero)

    // braking related variables
    float brake_gain;                           // gain used during conversion of vehicle's velocity to lean angle during braking (calculated from brake_rate)
    int16_t brake_roll;                         // target roll angle during braking periods
    int16_t brake_pitch;                        // target pitch angle during braking periods
    int16_t brake_timeout_roll;                 // number of cycles allowed for the braking to complete, this timeout will be updated at half-braking
    int16_t brake_timeout_pitch;                // number of cycles allowed for the braking to complete, this timeout will be updated at half-braking
    int16_t brake_angle_max_roll;               // maximum lean angle achieved during braking.  Used to determine when the vehicle has begun to flatten out so that we can re-estimate the braking time
    int16_t brake_angle_max_pitch;              // maximum lean angle achieved during braking  Used to determine when the vehicle has begun to flatten out so that we can re-estimate the braking time
    int16_t brake_to_loiter_timer;              // cycles to mix brake and loiter controls in HYBRID_BRAKE_TO_LOITER

    // loiter related variables
    int16_t loiter_to_pilot_timer_roll;         // cycles to mix loiter and pilot controls in HYBRID_LOITER_TO_PILOT
    int16_t loiter_to_pilot_timer_pitch;        // cycles to mix loiter and pilot controls in HYBRID_LOITER_TO_PILOT
    int16_t loiter_final_roll;                  // final roll angle from loiter controller as we exit loiter mode (used for mixing with pilot input)
    int16_t loiter_final_pitch;                 // final pitch angle from loiter controller as we exit loiter mode (used for mixing with pilot input)

    // wind compensation related variables
    Vector2f wind_comp_ef;                      // wind compensation in earth frame, filtered lean angles from position controller
    int16_t wind_comp_roll;                     // roll angle to compensate for wind
    int16_t wind_comp_pitch;                    // pitch angle to compensate for wind
    int8_t  wind_comp_timer;                    // counter to reduce wind_offset calcs to 10hz

    // final output
    int16_t roll;   // final roll angle sent to attitude controller
    int16_t pitch;  // final pitch angle sent to attitude controller
} hybrid;

// hybrid_init - initialise hybrid controller
static bool hybrid_init(bool ignore_checks)
{
    // fail to initialise hybrid mode if no GPS lock
    if (!GPS_ok() && !ignore_checks) {
        return false;
    }

    // set target to current position
    wp_nav.init_loiter_target();

    // clear pilot's feed forward for roll and pitch
    wp_nav.set_pilot_desired_acceleration(0, 0);

    // initialise altitude target to stopping point
    pos_control.set_target_to_stopping_point_z();

    // initialise lean angles to current attitude
    hybrid.pilot_roll = 0;
    hybrid.pilot_pitch = 0;
    hybrid.roll = constrain_int16(ahrs.roll_sensor, -g.hybrid_brake_angle_max, g.hybrid_brake_angle_max);
    hybrid.pitch = constrain_int16(ahrs.pitch_sensor, -g.hybrid_brake_angle_max, g.hybrid_brake_angle_max);

    // compute brake_gain
    hybrid.brake_gain = (15.0f * (float)g.hybrid_brake_rate + 95.0f) / 100.0f;

    if (ap.land_complete) {
        // if landed begin in loiter mode
        hybrid.roll_mode = HYBRID_LOITER;
        hybrid.pitch_mode = HYBRID_LOITER;
    }else{
        // if not landed start in pilot override to avoid hard twitch
        hybrid.roll_mode = HYBRID_PILOT_OVERRIDE;
        hybrid.pitch_mode = HYBRID_PILOT_OVERRIDE;
    }

    // initialise wind_comp each time hybrid is switched on
    hybrid.wind_comp_ef.zero();
    hybrid.wind_comp_roll = 0;
    hybrid.wind_comp_pitch = 0;
    hybrid.wind_comp_timer = 0;

    return true;
}

// hybrid_run - runs the hybrid controller
// should be called at 100hz or more
static void hybrid_run()
{
    int16_t target_roll, target_pitch;  // pilot's roll and pitch angle inputs
    float target_yaw_rate = 0;          // pilot desired yaw rate in centi-degrees/sec
    float target_climb_rate = 0;        // pilot desired climb rate in centimeters/sec
    float brake_to_loiter_mix;          // mix of brake and loiter controls.  0 = fully brake controls, 1 = fully loiter controls
    float loiter_to_pilot_mix;          // mix of loiter and pilot controls.  0 = fully loiter controls, 1 = fully pilot controls
    float vel_fw, vel_right;            // vehicle's current velocity in body-frame forward and right directions
    const Vector3f& vel = inertial_nav.get_velocity();

    // if not auto armed set throttle to zero and exit immediately
    if(!ap.auto_armed || !inertial_nav.position_ok()) {
        wp_nav.init_loiter_target();
        attitude_control.init_targets();
        attitude_control.set_throttle_out(0, false);
        return;
    }

    // process pilot inputs
    if (!failsafe.radio) {
        // apply SIMPLE mode transform to pilot inputs
        update_simple_mode();

        // get pilot's desired yaw rate
        target_yaw_rate = get_pilot_desired_yaw_rate(g.rc_4.control_in);

        // get pilot desired climb rate (for alt-hold mode and take-off)
        target_climb_rate = get_pilot_desired_climb_rate(g.rc_3.control_in);

        // check for pilot requested take-off
        if (ap.land_complete && target_climb_rate > 0) {
            // indicate we are taking off
            set_land_complete(false);
            // clear i term when we're taking off
            set_throttle_takeoff();
        }
    }

    // if landed initialise loiter targets, set throttle to zero and exit
    if (ap.land_complete) {
        wp_nav.init_loiter_target();
        attitude_control.init_targets();
        attitude_control.set_throttle_out(0, false);
        return;
    }else{
        // convert pilot input to lean angles
        get_pilot_desired_lean_angles(g.rc_1.control_in, g.rc_2.control_in, target_roll, target_pitch);

        // convert inertial nav earth-frame velocities to body-frame
        // To-Do: move this to AP_Math (or perhaps we already have a function to do this)
        vel_fw = vel.x*ahrs.cos_yaw() + vel.y*ahrs.sin_yaw();
        vel_right = -vel.x*ahrs.sin_yaw() + vel.y*ahrs.cos_yaw();

        // Roll state machine
        //  Each state (aka mode) is responsible for:
        //      1. dealing with pilot input
        //      2. calculating the final roll output to the attitude controller
        //      3. checking if the state (aka mode) should be changed and if 'yes' perform any required initialisation for the new state
        switch (hybrid.roll_mode) {

            case HYBRID_PILOT_OVERRIDE:
                // update pilot desired roll angle using latest radio input
                //  this filters the input so that it returns to zero no faster than the brake-rate
                hybrid_update_pilot_lean_angle(hybrid.pilot_roll, target_roll);

                // switch to BRAKE mode for next iteration if no pilot input
                if ((target_roll == 0) && (abs(hybrid.pilot_roll) < 2 * g.hybrid_brake_rate)) {
                    // initialise BRAKE mode
                    hybrid.roll_mode = HYBRID_BRAKE;        // Set brake roll mode
                    hybrid.brake_roll = 0;                  // initialise braking angle to zero
                    hybrid.brake_angle_max_roll = 0;        // reset brake_angle_max so we can detect when vehicle begins to flatten out during braking
                    hybrid.brake_timeout_roll = HYBRID_BRAKE_TIME_ESTIMATE_MAX; // number of cycles the brake will be applied, updated during braking mode.  To-Do: this must be adjusted based on loop rate
                    hybrid.braking_time_updated_roll = false;   // flag the braking time can be re-estimated
                }

                // final lean angle should be pilot input plus wind compensation
                hybrid.roll = hybrid.pilot_roll + hybrid.wind_comp_roll;
                break;

            case HYBRID_BRAKE:
            case HYBRID_BRAKE_READY_TO_LOITER:
                // calculate brake_roll angle to counter-act velocity
                hybrid_update_brake_angle_from_velocity(hybrid.brake_roll, vel_right);

                // update braking time estimate
                if (!hybrid.braking_time_updated_roll) {
                    // check if brake angle is increasing
                    if (abs(hybrid.brake_roll) >= hybrid.brake_angle_max_roll) {
                        hybrid.brake_angle_max_roll = abs(hybrid.brake_roll);
                    } else {
                        // braking angle has started decreasing so re-estimate braking time
                        hybrid.brake_timeout_roll = 1+(uint16_t)(15L*(int32_t)(abs(hybrid.brake_roll))/(10L*(int32_t)g.hybrid_brake_rate));  // the 1.2 (12/10) factor has to be tuned in flight, here it means 120% of the "normal" time.
                        hybrid.braking_time_updated_roll = true;
                    }
                }

                // if velocity is very low reduce braking time to 0.5seconds
                if ((fabs(vel_right) <= HYBRID_SPEED_0) && (hybrid.brake_timeout_roll > 50)) {
                    hybrid.brake_timeout_roll = 50;
                }

                // reduce braking timer
                if (hybrid.brake_timeout_roll > 0) {
                    hybrid.brake_timeout_roll--;
                } else {
                    // indicate that we are ready to move to Loiter.
                    // Loiter will only actually be engaged once both roll_mode and pitch_mode are changed to HYBRID_BRAKE_READY_TO_LOITER
                    //  logic for engaging loiter is handled below the roll and pitch mode switch statements
                    hybrid.roll_mode = HYBRID_BRAKE_READY_TO_LOITER;
                }

                // check for pilot input
                if (target_roll != 0) {
                    hybrid.roll_mode = HYBRID_PILOT_OVERRIDE;
                }

                // final lean angle is braking angle + wind compensation angle
                hybrid.roll = hybrid.brake_roll + hybrid.wind_comp_roll;
                break;

            case HYBRID_BRAKE_TO_LOITER:
            case HYBRID_LOITER:
                // these modes are combined roll-pitch modes and are handled below
                break;

            case HYBRID_LOITER_TO_PILOT_OVERRIDE:
                // update pilot desired roll angle using latest radio input
                //  this filters the input so that it returns to zero no faster than the brake-rate
                hybrid_update_pilot_lean_angle(hybrid.pilot_roll, target_roll);

                // count-down loiter to pilot timer
                if (hybrid.loiter_to_pilot_timer_roll > 0) {
                    hybrid.loiter_to_pilot_timer_roll--;
                } else {
                    // when timer runs out switch to full pilot override for next iteration
                    hybrid.roll_mode = HYBRID_PILOT_OVERRIDE;
                }

                // calculate loiter_to_pilot mix ratio
                loiter_to_pilot_mix = (float)hybrid.loiter_to_pilot_timer_roll / (float)HYBRID_LOITER_TO_PILOT_MIX_TIMER;

                // mix final loiter lean angle and pilot desired lean angles
                hybrid.roll = hybrid_mix_controls(loiter_to_pilot_mix, hybrid.loiter_final_roll, hybrid.pilot_roll + hybrid.wind_comp_roll);
                break;
        }

        // Pitch state machine
        //  Each state (aka mode) is responsible for:
        //      1. dealing with pilot input
        //      2. calculating the final pitch output to the attitude contpitcher
        //      3. checking if the state (aka mode) should be changed and if 'yes' perform any required initialisation for the new state
        switch (hybrid.pitch_mode) {

            case HYBRID_PILOT_OVERRIDE:
                // update pilot desired pitch angle using latest radio input
                //  this filters the input so that it returns to zero no faster than the brake-rate
                hybrid_update_pilot_lean_angle(hybrid.pilot_pitch, target_pitch);

                // switch to BRAKE mode for next iteration if no pilot input
                if ((target_pitch == 0) && (abs(hybrid.pilot_pitch) < 2 * g.hybrid_brake_rate)) {
                    // initialise BRAKE mode
                    hybrid.pitch_mode = HYBRID_BRAKE;       // set brake pitch mode
                    hybrid.brake_pitch = 0;                 // initialise braking angle to zero
                    hybrid.brake_angle_max_pitch = 0;       // reset brake_angle_max so we can detect when vehicle begins to flatten out during braking
                    hybrid.brake_timeout_pitch = HYBRID_BRAKE_TIME_ESTIMATE_MAX; // number of cycles the brake will be applied, updated during braking mode.  To-Do: this must be adjusted based on loop rate
                    hybrid.braking_time_updated_pitch = false;   // flag the braking time can be re-estimated
                }

                // final lean angle should be pilot input plus wind compensation
                hybrid.pitch = hybrid.pilot_pitch + hybrid.wind_comp_pitch;
                break;

            case HYBRID_BRAKE:
            case HYBRID_BRAKE_READY_TO_LOITER:
                // calculate brake_pitch angle to counter-act velocity
                hybrid_update_brake_angle_from_velocity(hybrid.brake_pitch, -vel_fw);

                // update braking time estimate
                if (!hybrid.braking_time_updated_pitch) {
                    // check if brake angle is increasing
                    if (abs(hybrid.brake_pitch) >= hybrid.brake_angle_max_pitch) {
                        hybrid.brake_angle_max_pitch = abs(hybrid.brake_pitch);
                    } else {
                        // braking angle has started decreasing so re-estimate braking time
                        hybrid.brake_timeout_pitch = 1+(uint16_t)(15L*(int32_t)(abs(hybrid.brake_pitch))/(10L*(int32_t)g.hybrid_brake_rate));  // the 1.2 (12/10) factor has to be tuned in flight, here it means 120% of the "normal" time.
                        hybrid.braking_time_updated_pitch = true;
                    }
                }

                // if velocity is very low reduce braking time to 0.5seconds
                //  Note: this speed is extremely low (only 10cm/s) meaning this case is likely never executed
                if ((fabs(vel_right) <= HYBRID_SPEED_0) && (hybrid.brake_timeout_pitch > 50)) {
                    hybrid.brake_timeout_pitch = 50;
                }

                // reduce braking timer
                if (hybrid.brake_timeout_pitch > 0) {
                    hybrid.brake_timeout_pitch--;
                } else {
                    // indicate that we are ready to move to Loiter.
                    // Loiter will only actually be engaged once both pitch_mode and pitch_mode are changed to HYBRID_BRAKE_READY_TO_LOITER
                    //  logic for engaging loiter is handled below the pitch and pitch mode switch statements
                    hybrid.pitch_mode = HYBRID_BRAKE_READY_TO_LOITER;
                }

                // check for pilot input
                if (target_pitch != 0) {
                    hybrid.pitch_mode = HYBRID_PILOT_OVERRIDE;
                }

                // final lean angle is braking angle + wind compensation angle
                hybrid.pitch = hybrid.brake_pitch + hybrid.wind_comp_pitch;
                break;

            case HYBRID_BRAKE_TO_LOITER:
            case HYBRID_LOITER:
                // these modes are combined pitch-pitch modes and are handled below
                break;

            case HYBRID_LOITER_TO_PILOT_OVERRIDE:
                // update pilot desired pitch angle using latest radio input
                //  this filters the input so that it returns to zero no faster than the brake-rate
                hybrid_update_pilot_lean_angle(hybrid.pilot_pitch, target_pitch);

                // count-down loiter to pilot timer
                if (hybrid.loiter_to_pilot_timer_pitch > 0) {
                    hybrid.loiter_to_pilot_timer_pitch--;
                } else {
                    // when timer runs out switch to full pilot override for next iteration
                    hybrid.pitch_mode = HYBRID_PILOT_OVERRIDE;
                }

                // calculate loiter_to_pilot mix ratio
                loiter_to_pilot_mix = (float)hybrid.loiter_to_pilot_timer_pitch / (float)HYBRID_LOITER_TO_PILOT_MIX_TIMER;

                // mix final loiter lean angle and pilot desired lean angles
                hybrid.pitch = hybrid_mix_controls(loiter_to_pilot_mix, hybrid.loiter_final_pitch, hybrid.pilot_pitch + hybrid.wind_comp_pitch);
                break;
        }

        //
        // Shared roll & pitch states (HYBRID_BRAKE_TO_LOITER and HYBRID_LOITER)
        //

        // switch into LOITER mode when both roll and pitch are ready
        if (hybrid.roll_mode == HYBRID_BRAKE_READY_TO_LOITER && hybrid.pitch_mode == HYBRID_BRAKE_READY_TO_LOITER) {
            hybrid.roll_mode = HYBRID_BRAKE_TO_LOITER;
            hybrid.pitch_mode = HYBRID_BRAKE_TO_LOITER;
            hybrid.brake_to_loiter_timer = HYBRID_BRAKE_TO_LOITER_TIMER;
            // init loiter controller
            wp_nav.init_loiter_target();
            // move wind compensation back into loiter I term
            g.pid_loiter_rate_lat.set_integrator(hybrid.wind_comp_ef.x);
            g.pid_loiter_rate_lon.set_integrator(hybrid.wind_comp_ef.y);
        }

        // roll-mode is used as the combined roll+pitch mode when in BRAKE_TO_LOITER or LOITER modes
        if (hybrid.roll_mode == HYBRID_BRAKE_TO_LOITER || hybrid.roll_mode == HYBRID_LOITER) {

            // force pitch mode to be same as roll_mode just to keep it consistent (it's not actually used in these states)
            hybrid.pitch_mode = hybrid.roll_mode;

            // handle combined roll+pitch mode
            switch (hybrid.roll_mode) {
                case HYBRID_BRAKE_TO_LOITER:
                    // reduce brake_to_loiter timer
                    if (hybrid.brake_to_loiter_timer > 0) {
                        hybrid.brake_to_loiter_timer--;
                    } else {
                        // progress to full loiter on next iteration
                        hybrid.roll_mode = HYBRID_LOITER;
                        hybrid.pitch_mode = HYBRID_LOITER;
                    }

                    // calculate percentage mix of loiter and brake control
                    brake_to_loiter_mix = (float)hybrid.brake_to_loiter_timer / (float)HYBRID_BRAKE_TO_LOITER_TIMER;

                    // calculate brake_roll and pitch angles to counter-act velocity
                    hybrid_update_brake_angle_from_velocity(hybrid.brake_roll, vel_right);
                    hybrid_update_brake_angle_from_velocity(hybrid.brake_pitch, -vel_fw);

                    // run loiter controller
                    wp_nav.update_loiter();

                    // calculate final roll and pitch output by mixing loiter and brake controls
                    hybrid.roll = hybrid_mix_controls(brake_to_loiter_mix, hybrid.brake_roll + hybrid.wind_comp_roll, wp_nav.get_roll());
                    hybrid.pitch = hybrid_mix_controls(brake_to_loiter_mix, hybrid.brake_pitch + hybrid.wind_comp_pitch, wp_nav.get_pitch());

                    // check for pilot input
                    if (target_roll != 0 || target_pitch != 0) {
                        // if roll input switch to pilot override for roll
                        if (target_roll != 0) {
                            hybrid.roll_mode = HYBRID_PILOT_OVERRIDE;
                            // switch pitch-mode to brake (but ready to go back to loiter anytime)
                            hybrid.pitch_mode = HYBRID_BRAKE_READY_TO_LOITER;
                        }
                        // if pitch input switch to pilot override for pitch
                        if (target_pitch != 0) {
                            hybrid.pitch_mode = HYBRID_PILOT_OVERRIDE;
                            if (target_roll == 0) {
                                // switch roll-mode to brake (but ready to go back to loiter anytime)
                                hybrid.roll_mode = HYBRID_BRAKE_READY_TO_LOITER;
                            }
                        }
                    }
                    break;

                case HYBRID_LOITER:
                    // run loiter controller
                    wp_nav.update_loiter();

                    // set roll angle based on loiter controller outputs
                    hybrid.roll = wp_nav.get_roll();
                    hybrid.pitch = wp_nav.get_pitch();

                    // update wind compensation estimate
                    hybrid_update_wind_comp_estimate();

                    // check for pilot input
                    if (target_roll != 0 || target_pitch != 0) {
                        // if roll input switch to pilot override for roll
                        if (target_roll != 0) {
                            hybrid.roll_mode = HYBRID_LOITER_TO_PILOT_OVERRIDE;
                            hybrid.loiter_to_pilot_timer_roll = HYBRID_LOITER_TO_PILOT_MIX_TIMER;
                            // initialise pilot_roll
                            hybrid.pilot_roll = target_roll;
                            // switch pitch-mode to brake (but ready to go back to loiter anytime)
                            hybrid.pitch_mode = HYBRID_BRAKE_READY_TO_LOITER;
                        }
                        // if pitch input switch to pilot override for pitch
                        if (target_pitch != 0) {
                            hybrid.pitch_mode = HYBRID_LOITER_TO_PILOT_OVERRIDE;
                            hybrid.loiter_to_pilot_timer_pitch = HYBRID_LOITER_TO_PILOT_MIX_TIMER;
                            // initialise pilot_pitch
                            hybrid.pilot_pitch = target_pitch;
                            // if roll not overriden switch roll-mode to brake (but be ready to go back to loiter any time)
                            if (target_roll == 0) {
                                hybrid.roll_mode = HYBRID_BRAKE_READY_TO_LOITER;
                            }
                        }
                        // store final loiter outputs for mixing with pilot input
                        hybrid.loiter_final_roll = wp_nav.get_roll();
                        hybrid.loiter_final_pitch = wp_nav.get_pitch();
                        // retrieve latest wind compensation lean angles
                        hybrid_get_wind_comp_lean_angles(hybrid.wind_comp_roll, hybrid.wind_comp_pitch);
                    }
                    break;

                default:
                    // do nothing for uncombined roll and pitch modes
                    break;
            }
        }

        // constrain target pitch/roll angles
        hybrid.roll = constrain_int16(hybrid.roll, -aparm.angle_max, aparm.angle_max);
        hybrid.pitch = constrain_int16(hybrid.pitch, -aparm.angle_max, aparm.angle_max);

        // update attitude controller targets
        attitude_control.angle_ef_roll_pitch_rate_ef_yaw(hybrid.roll, hybrid.pitch, target_yaw_rate);

        // throttle control
        if (sonar_alt_health >= SONAR_ALT_HEALTH_MAX) {
            // if sonar is ok, use surface tracking
            target_climb_rate = get_throttle_surface_tracking(target_climb_rate, pos_control.get_alt_target(), G_Dt);
        }
        // update altitude target and call position controller
        pos_control.set_alt_target_from_climb_rate(target_climb_rate, G_Dt);
        pos_control.update_z_controller();
    }
}

// hybrid_update_pilot_lean_angle - update the pilot's filtered lean angle with the latest raw input received
static void hybrid_update_pilot_lean_angle(int16_t &lean_angle_filtered, int16_t &lean_angle_raw)
{
    // if raw input is large or reversing the vehicle's lean angle immediately set the fitlered angle to the new raw angle
    if ((lean_angle_filtered > 0 && lean_angle_raw < 0) || (lean_angle_filtered < 0 && lean_angle_raw > 0) || (abs(lean_angle_raw) > HYBRID_STICK_RELEASE_SMOOTH_ANGLE)) {
        lean_angle_filtered = lean_angle_raw;
    } else {
        // lean_angle_raw must be pulling lean_angle_filtered towards zero, smooth the decrease
        if (lean_angle_filtered > 0) {
            // reduce the filtered lean angle at 4% or the brake rate (whichever is faster).
            // To-Do: the HYBRID_SMOOTH_RATE_FACTOR must be adjusted based on update rate
            lean_angle_filtered -= max((float)lean_angle_filtered * HYBRID_SMOOTH_RATE_FACTOR, g.hybrid_brake_rate);
            // do not let the filtered angle fall below the pilot's input lean angle.
            // the above line pulls the filtered angle down and the below line acts as a catch
            lean_angle_filtered = max(lean_angle_filtered, lean_angle_raw);
        }else{
            // To-Do: the HYBRID_SMOOTH_RATE_FACTOR must be adjusted based on update rate
            lean_angle_filtered += max(-(float)lean_angle_filtered * HYBRID_SMOOTH_RATE_FACTOR, g.hybrid_brake_rate);
            lean_angle_filtered = min(lean_angle_filtered, lean_angle_raw);
        }
    }
}

// hybrid_mix_controls - mixes two controls based on the mix_ratio
//  mix_ratio of 1 = use first_control completely, 0 = use second_control completely, 0.5 = mix evenly
static int16_t hybrid_mix_controls(float mix_ratio, int16_t first_control, int16_t second_control)
{
    mix_ratio = constrain_float(mix_ratio, 0.0f, 1.0f);
    return (int16_t)((mix_ratio * first_control) + ((1.0f-mix_ratio)*second_control));
}

// hybrid_update_brake_angle_from_velocity - updates the brake_angle based on the vehicle's velocity and brake_gain
//  brake_angle is slewed with the wpnav.hybrid_brake_rate and constrained by the wpnav.hybrid_braking_angle_max
//  velocity is assumed to be in the same direction as lean angle so for pitch you should provide the velocity backwards (i.e. -ve forward velocity)
static void hybrid_update_brake_angle_from_velocity(int16_t &brake_angle, float velocity)
{
    float lean_angle;

    // calculate velocity-only based lean angle
    if (velocity >= 0) {
        lean_angle = -hybrid.brake_gain * velocity * (1.0f+500.0f/(velocity+60.0f));
    } else {
        lean_angle = -hybrid.brake_gain * velocity * (1.0f+500.0f/(-velocity+60.0f));
    }

    // do not let lean_angle be too far from brake_angle
    // To-Do: this constraint needs to account for loop update rate
    brake_angle = constrain_int16((int16_t)lean_angle, brake_angle - g.hybrid_brake_rate, brake_angle + g.hybrid_brake_rate);

    // constrain final brake_angle
    brake_angle = constrain_int16(brake_angle, -g.hybrid_brake_angle_max, g.hybrid_brake_angle_max);
}

// hybrid_update_wind_comp_estimate - updates wind compensation estimate
//  should be called at the maximum loop rate when loiter is engaged
//  To-Do: adjust the filtering for 100hz and 400hz update rates
static void hybrid_update_wind_comp_estimate()
{
    if (inertial_nav.get_velocity_xy() > HYBRID_WIND_COMP_ESTIMATE_SPEED_MAX) {
        return;
    }

    // get position controller accel target
    //  To-Do: clean this up by using accessor in loiter controller (or move entire hybrid controller to a library shared with loiter)
    const Vector3f& accel_target = pos_control.get_accel_target();

    // update wind compensation in earth-frame lean angles
    if (hybrid.wind_comp_ef.x == 0) {
        // if wind compensation has not been initialised set it immediately to the pos controller's desired accel in north direction
        hybrid.wind_comp_ef.x = accel_target.x;
    } else {
        // low pass filter the position controller's lean angle output
        hybrid.wind_comp_ef.x = 0.97f*hybrid.wind_comp_ef.x + 0.03f*accel_target.x;
    }
    if (hybrid.wind_comp_ef.y == 0) {
        // if wind compensation has not been initialised set it immediately to the pos controller's desired accel in north direction
        hybrid.wind_comp_ef.y = accel_target.y;
    } else {
        // low pass filter the position controller's lean angle output
        hybrid.wind_comp_ef.y = 0.97f*hybrid.wind_comp_ef.y + 0.03f*accel_target.y;
    }
}

// hybrid_get_wind_comp_lean_angles - retrieve wind compensation angles in body frame roll and pitch angles
static void hybrid_get_wind_comp_lean_angles(int16_t &roll_angle, int16_t &pitch_angle)
{
    // convert earth frame desired accelerations to body frame roll and pitch lean angles
    roll_angle = (float)fast_atan((-hybrid.wind_comp_ef.x*ahrs.sin_yaw() + hybrid.wind_comp_ef.y*ahrs.cos_yaw())/981)*(18000/M_PI);
    pitch_angle = (float)fast_atan(-(hybrid.wind_comp_ef.x*ahrs.cos_yaw() + hybrid.wind_comp_ef.y*ahrs.sin_yaw())/981)*(18000/M_PI);
}
