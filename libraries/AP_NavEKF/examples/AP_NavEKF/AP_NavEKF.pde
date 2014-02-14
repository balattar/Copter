/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
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

#include <AP_Common.h>
#include <AP_Progmem.h>
#include <AP_Param.h>
#include <AP_Math.h>
#include <AP_HAL.h>
#include <AP_HAL_AVR.h>
#include <AP_HAL_AVR_SITL.h>
#include <AP_HAL_Linux.h>
#include <AP_HAL_Empty.h>
#include <AP_ADC.h>
#include <AP_Declination.h>
#include <AP_ADC_AnalogSource.h>
#include <Filter.h>
#include <AP_Buffer.h>
#include <AP_Airspeed.h>
#include <AP_Vehicle.h>
#include <AP_Notify.h>
#include <DataFlash.h>
#include <GCS_MAVLink.h>
#include <AP_GPS.h>
#include <AP_AHRS.h>
#include <SITL.h>
#include <AP_Compass.h>
#include <AP_Baro.h>
#include <AP_InertialSensor.h>
#include <AP_NavEKF.h>
#include <stdio.h>

#include "LogReader.h"

const AP_HAL::HAL& hal = AP_HAL_BOARD_DRIVER;

static AP_InertialSensor_HIL ins;
static AP_Baro_HIL barometer;
static AP_GPS_HIL gps_driver;
static GPS *g_gps = &gps_driver;
static AP_Compass_HIL compass;
static AP_AHRS_DCM ahrs(ins, g_gps);

#if CONFIG_HAL_BOARD == HAL_BOARD_AVR_SITL
SITL sitl;
#endif

static NavEKF NavEKF(ahrs, barometer);
static LogReader LogReader(ins, barometer, compass, g_gps);

static void delay_cb()
{
    LogReader.wait_type(LOG_GPS_MSG);
}

static FILE *plotf;

void setup()
{
    ::printf("Starting\n");
    LogReader.open_log("log.bin");

    LogReader.wait_type(LOG_GPS_MSG);
    LogReader.wait_type(LOG_IMU_MSG);
    LogReader.wait_type(LOG_GPS_MSG);
    LogReader.wait_type(LOG_IMU_MSG);

    hal.scheduler->register_delay_callback(delay_cb, 5);

    ahrs.set_compass(&compass);
    ahrs.set_fly_forward(true);
    ahrs.set_wind_estimation(true);
    
    barometer.init();
    compass.init();
    ins.init(AP_InertialSensor::WARM_START, AP_InertialSensor::RATE_50HZ);

    ::printf("Waiting for 3D fix\n");
    uint8_t goodFrameCount = 0;
    while (goodFrameCount <= 10) // wait for readings to stabilise
    {
        LogReader.wait_type(LOG_GPS_MSG);
        g_gps->update();
        compass.read();
        barometer.read();
        LogReader.wait_type(LOG_IMU_MSG);
        ahrs.update();
        if ((g_gps->status() >= GPS::GPS_OK_FIX_3D) && (ahrs.yaw_sensor != 0)) goodFrameCount +=1;
    }

    barometer.calibrate();
    compass.set_initial_location(g_gps->latitude, g_gps->longitude);

    NavEKF.InitialiseFilter();

    plotf = fopen("plot.dat", "w");
    fprintf(plotf, "time ATT.Roll ATT.Pitch ATT.Yaw AHRS.Roll AHRS.Pitch AHRS.Yaw EKF.Roll EKF.Pitch EKF.Yaw\n");
}

void loop()
{
    while (true) {
        uint8_t type;
        if (!LogReader.update(type)) {
            ::printf("End of log\n");
            fclose(plotf);
            exit(0);
        }
        switch (type) {
        case LOG_GPS_MSG:
            g_gps->update();
            barometer.read();
            break;

        case LOG_ATTITUDE_MSG: {
            Vector3f ekf_euler;
            NavEKF.getEulerAngles(ekf_euler);
            fprintf(plotf, "%.3f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f\n",
                    hal.scheduler->millis() * 0.001f,
                    LogReader.get_attitude().x,
                    LogReader.get_attitude().y,
                    LogReader.get_attitude().z,
                    ahrs.roll_sensor*0.01f, 
                    ahrs.pitch_sensor*0.01f,
                    ahrs.yaw_sensor*0.01f,
                    degrees(ekf_euler.x),
                    degrees(ekf_euler.y),
                    degrees(ekf_euler.z));
                    
                break;
        }

        case LOG_COMPASS_MSG:
            compass.read();
            break;

        case LOG_IMU_MSG:
            ahrs.update();
            NavEKF.UpdateFilter();
            break;
        }
    }
}

AP_HAL_MAIN();
