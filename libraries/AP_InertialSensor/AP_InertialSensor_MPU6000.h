/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef __AP_INERTIAL_SENSOR_MPU6000_H__
#define __AP_INERTIAL_SENSOR_MPU6000_H__

#include <stdint.h>
#include <AP_HAL.h>
#include <AP_Math.h>
#include <AP_Progmem.h>
#include "AP_InertialSensor.h"

#define MPU6000_CS_PIN       53        // APM pin connected to mpu6000's chip select pin

// enable debug to see a register dump on startup
#define MPU6000_DEBUG 0

class AP_InertialSensor_MPU6000 : public AP_InertialSensor
{
public:

    AP_InertialSensor_MPU6000();

    /* Concrete implementation of AP_InertialSensor functions: */
    bool                update();
    float               get_gyro_drift_rate();

    // sample_available - true when a new sample is available
    bool                sample_available();

    // get_delta_time returns the time period in seconds overwhich the sensor data was collected
    float            	get_delta_time();

protected:
    uint16_t                    _init_sensor( Sample_rate sample_rate );

private:

    static void                 _read_data_from_timerprocess();
    static void                 _read_data_transaction();
    static bool                 _data_ready();
    static void                 _poll_data(uint32_t now);
    static AP_HAL::DigitalSource *_drdy_pin;
    static uint8_t              _register_read( uint8_t reg );
    static bool _register_read_from_timerprocess( uint8_t reg, uint8_t *val );
    static void                 register_write( uint8_t reg, uint8_t val );
    void                        wait_for_sample();
    bool                        hardware_init(Sample_rate sample_rate);

    static AP_HAL::SPIDeviceDriver *_spi;
    static AP_HAL::Semaphore *_spi_sem;

    uint16_t					_num_samples;

    float                       _temp;

    float                       _temp_to_celsius( uint16_t );

    static const float          _gyro_scale;

    static const uint8_t        _gyro_data_index[3];
    static const int8_t         _gyro_data_sign[3];

    static const uint8_t        _accel_data_index[3];
    static const int8_t         _accel_data_sign[3];

    static const uint8_t        _temp_data_index;

    // ensure we can't initialise twice
    bool                        _initialised;
    static int16_t              _mpu6000_product_id;

    // how many hardware samples before we report a sample to the caller
    uint8_t _sample_shift;

    // support for updating filter at runtime
    uint8_t _last_filter_hz;

    void _set_filter_register(uint8_t filter_hz, uint8_t default_filter);

public:

#if MPU6000_DEBUG
    void						_dump_registers(void);
#endif
};

#endif // __AP_INERTIAL_SENSOR_MPU6000_H__
