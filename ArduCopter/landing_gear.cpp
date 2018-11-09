#include "Copter.h"


// Run landing gear controller at 10Hz
void Copter::landinggear_update()
{
    // exit immediately if no landing gear output has been enabled
    if (!SRV_Channels::function_assigned(SRV_Channel::k_landing_gear_control)) {
        return;
    }

    // last status (deployed or retracted) used to check for changes, initialised to startup state of landing gear
    static bool last_deploy_status = landinggear.deployed();

    // if we are doing an automatic landing procedure, force the landing gear to deploy.
    // To-Do: should we pause the auto-land procedure to give time for gear to come down?
    if (flightmode->landing_gear_should_be_deployed()) {
        landinggear.set_position(AP_LandingGear::LandingGear_Deploy);
    }

    // send event message to datalog if status has changed
    if (landinggear.deployed() != last_deploy_status) {
        if (landinggear.deployed()) {
            Log_Write_Event(DATA_LANDING_GEAR_DEPLOYED);
        } else {
            Log_Write_Event(DATA_LANDING_GEAR_RETRACTED);
        }
    }

    last_deploy_status = landinggear.deployed();

    // support height based triggering
    float height;
    if (rangefinder_alt_ok()) {
        height = rangefinder_state.alt_cm_filt.get() * 0.01;
    } else {
        height = current_loc.alt * 0.01;
    }

    landinggear.update(height);
}
