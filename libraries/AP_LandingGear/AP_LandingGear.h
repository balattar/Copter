// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

/// @file	AP_LandingGear.h
/// @brief	Landing gear control library

#ifndef AP_LANDINGGEAR_H
#define AP_LANDINGGEAR_H

#include <AP_Param.h>
#include <AP_Common.h>

#define AP_LANDINGGEAR_SERVO_RETRACT_PWM_DEFAULT    1250    // default PWM value to move servo to when landing gear is up
#define AP_LANDINGGEAR_SERVO_DEPLOY_PWM_DEFAULT     1750    // default PWM value to move servo to when landing gear is down

#define COMMAND_MODE_DEPLOY                         0       // command gear to deploy
#define COMMAND_MODE_AUTO                           1       // command gear to actuate automatically
#define COMMAND_MODE_RETRACT                        2       // command gear to retract

/// @class	AP_LandingGear
/// @brief	Class managing the control of landing gear
class AP_LandingGear {

public:

    /// Constructor
    AP_LandingGear() :
        _deployed(false),
        _retract_enabled(false)
    {
        // setup parameter defaults
        AP_Param::setup_object_defaults(this, var_info);
    }

    /// enabled - returns true if landing gear retract is enabled
    bool enabled() const { return _retract_enabled; }
    
    /// deployed - returns true if the landing gear is deployed
    bool deployed() const { return _deployed; }

    /// update - should be called at 10hz
    void update();
    
    void set_cmd_mode(int8_t cmd) { _command_mode = cmd; }

    static const struct AP_Param::GroupInfo        var_info[];

private:

    bool     _retract_enabled;          // true if landing gear retraction is enabled

    // Parameters
    AP_Int16    _servo_retract_pwm;     // PWM value to move servo to when gear is retracted
    AP_Int16    _servo_deploy_pwm;      // PWM value to move servo to when gear is deployed

    // internal variables
    bool        _deployed;              // true if the landing gear has been deployed, initialized false
    int8_t      _command_mode;          // pilots commanded control mode: Manual Deploy, Auto, or Manual Retract
    
    /// enable - enable landing gear retraction
    void enable(bool on_off);
    
    /// retract - retract landing gear
    void retract();
    
    /// deploy - deploy the landing gear
    void deploy();
};

#endif /* AP_LANDINGGEAR_H */
