// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

//
// Simple test for the AP_AHRS interface
//

#include <AP_ADC/AP_ADC.h>
#include <AP_ADC_AnalogSource/AP_ADC_AnalogSource.h>
#include <AP_AHRS/AP_AHRS.h>
#include <AP_HAL/AP_HAL.h>

const AP_HAL::HAL& hal = AP_HAL::get_HAL();

// INS and Baro declaration
AP_InertialSensor ins;

#if CONFIG_HAL_BOARD == HAL_BOARD_APM1
AP_ADC_ADS7844 apm1_adc;
#endif

Compass compass;

AP_GPS gps;
AP_Baro baro;
AP_SerialManager serial_manager;

// choose which AHRS system to use
AP_AHRS_DCM  ahrs(ins, baro, gps);



#define HIGH 1
#define LOW 0

void setup(void)
{

#ifdef APM2_HARDWARE
    // we need to stop the barometer from holding the SPI bus
    hal.gpio->pinMode(40, HAL_HAL_GPIO_OUTPUT);
    hal.gpio->write(40, HIGH);
#endif

    ins.init(AP_InertialSensor::RATE_100HZ);
    ahrs.init();
    serial_manager.init();

    if( compass.init() ) {
        hal.console->printf("Enabling compass\n");
        ahrs.set_compass(&compass);
    } else {
        hal.console->printf("No compass detected\n");
    }
    gps.init(NULL, serial_manager);
}

void loop(void)
{
    static uint16_t counter;
    static uint32_t last_t, last_print, last_compass;
    uint32_t now = hal.scheduler->micros();
    float heading = 0;

    if (last_t == 0) {
        last_t = now;
        return;
    }
    last_t = now;

    if (now - last_compass > 100*1000UL &&
        compass.read()) {
        heading = compass.calculate_heading(ahrs.get_dcm_matrix());
        // read compass at 10Hz
        last_compass = now;
#if WITH_GPS
        g_gps->update();
#endif
    }

    ahrs.update();
    counter++;

    if (now - last_print >= 100000 /* 100ms : 10hz */) {
        Vector3f drift  = ahrs.get_gyro_drift();
        hal.console->printf(
                "r:%4.1f  p:%4.1f y:%4.1f "
                    "drift=(%5.1f %5.1f %5.1f) hdg=%.1f rate=%.1f\n",
                        ToDeg(ahrs.roll),
                        ToDeg(ahrs.pitch),
                        ToDeg(ahrs.yaw),
                        ToDeg(drift.x),
                        ToDeg(drift.y),
                        ToDeg(drift.z),
                        compass.use_for_yaw() ? ToDeg(heading) : 0.0f,
                        (1.0e6f*counter)/(now-last_print));
        last_print = now;
        counter = 0;
    }
}

AP_HAL_MAIN();
