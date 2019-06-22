/* Variometer class by Samuel Tabor

Manages the estimation of aircraft total energy, drag and vertical air velocity.
*/
#include "Variometer.h"

#include <AP_Logger/AP_Logger.h>

Variometer::Variometer(AP_AHRS &ahrs, const AP_Vehicle::FixedWing &parms) :
    _ahrs(ahrs),
    _aparm(parms)
{
    _climb_filter = LowPassFilter<float>(1.0/60.0);
}

void Variometer::update(const float polar_K, const float polar_B, const float polar_Cd0)
{
    _ahrs.get_relative_position_D_home(alt);
    alt = -alt;

    // Logic borrowed from AP_TECS.cpp
    // Update and average speed rate of change
    // Get DCM
    const Matrix3f &rotMat = _ahrs.get_rotation_body_to_ned();
    // Calculate speed rate of change
    float temp = rotMat.c.x * GRAVITY_MSS + AP::ins().get_accel().x;
    // take 5 point moving average
    float dsp = _vdot_filter.apply(temp);

    Vector3f velned;
    if (_ahrs.get_velocity_NED(velned)) {
        // if possible use the EKF vertical velocity
        raw_climb_rate = -velned.z;
    }
    smoothed_climb_rate = _climb_filter.apply(raw_climb_rate, (AP_HAL::micros64() - _prev_update_time)/1e6);

    float aspd = 0;
    if (!_ahrs.airspeed_estimate(aspd)) {
            aspd = _aparm.airspeed_cruise_cm / 100.0f;
    }
    _aspd_filt = _sp_filter.apply(aspd);

    float roll = _ahrs.roll;

    // Constrained airspeed.
    const float minV = sqrt(polar_K/1.5);
    float aspd_filt_constrained = _aspd_filt>minV ? _aspd_filt : minV;


    // Compute still-air sinkrate
    float sinkrate = correct_netto_rate(0.0f,  roll, aspd_filt_constrained, polar_K, polar_Cd0, polar_B);

    reading = raw_climb_rate + dsp*aspd_filt_constrained/GRAVITY_MSS + sinkrate;
    

    filtered_reading = TE_FILT * reading + (1 - TE_FILT) * filtered_reading;                       // Apply low pass timeconst filter for noise
    displayed_reading = TE_FILT_DISPLAYED * reading + (1 - TE_FILT_DISPLAYED) * displayed_reading;

    _prev_update_time = AP_HAL::micros64();

    AP::logger().Write("VAR", "TimeUS,aspd_raw,aspd_filt,alt,roll,raw,filt,cl,fc", "Qffffffff",
                       AP_HAL::micros64(),
                       (double)0.0,
                       (double)aspd_filt_constrained,
                       (double)alt,
                       (double)roll,
                       (double)reading,
                       (double)filtered_reading,
                       (double)raw_climb_rate,
                       (double)smoothed_climb_rate);

    float expected_roll = asinf(constrain_float(powf(aspd_filt_constrained,2)/(GRAVITY_MSS*_aparm.loiter_radius),-1.0, 1.0));
    _expected_thermalling_sink = correct_netto_rate(0.0, expected_roll, aspd_filt_constrained, polar_K, polar_Cd0, polar_B);
}


float Variometer::correct_netto_rate(float climb_rate,
                                     float phi,
                                     float aspd,
                                     const float polar_K,
                                     const float polar_CD0,
                                     const float polar_B)
{
    // Remove aircraft sink rate
    float CL0;  // CL0 = 2*W/(rho*S*V^2)
    float C1;   // C1 = CD0/CL0
    float C2;   // C2 = CDi0/CL0 = B*CL0
    float netto_rate;
    float cosphi;
    CL0 = polar_K / (aspd * aspd);

    C1 = polar_CD0 / CL0;  // constant describing expected angle to overcome zero-lift drag
    C2 = polar_B * CL0;    // constant describing expected angle to overcome lift induced drag at zero bank

    cosphi = (1 - phi * phi / 2); // first two terms of mclaurin series for cos(phi)
    netto_rate = climb_rate + aspd * (C1 + C2 / (cosphi * cosphi));  // effect of aircraft drag removed

    // Remove acceleration effect - needs to be tested.
    //float temp_netto = netto_rate;
    //float dVdt = SpdHgt_Controller->get_VXdot();
    //netto_rate = netto_rate + aspd*dVdt/GRAVITY_MSS;
    //gcs().send_text(MAV_SEVERITY_INFO, "%f %f %f %f",temp_netto,dVdt,netto_rate,barometer.get_altitude());
    return netto_rate;
}
