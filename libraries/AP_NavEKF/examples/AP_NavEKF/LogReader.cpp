#include <AP_HAL.h>
#include <AP_Common.h>
#include <AP_Math.h>
#include <AP_Airspeed.h>
#include <AP_Compass.h>
#include <AP_GPS.h>
#include <AP_Compass.h>
#include <AP_Baro.h>
#include <AP_InertialSensor.h>
#include <DataFlash.h>

#include "LogReader.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern const AP_HAL::HAL& hal;

LogReader::LogReader(AP_InertialSensor &_ins, AP_Baro_HIL &_baro, AP_Compass_HIL &_compass, GPS *&_gps) :
    vehicle(VEHICLE_UNKNOWN),
    fd(-1),
    ins(_ins),
    baro(_baro),
    compass(_compass),
    gps(_gps)
{}

bool LogReader::open_log(const char *logfile)
{
    fd = ::open(logfile, O_RDONLY);
    if (fd == -1) {
        return false;
    }
    return true;
}


struct PACKED log_Plane_Compass {
    LOG_PACKET_HEADER;
    uint32_t time_ms;
    int16_t mag_x;
    int16_t mag_y;
    int16_t mag_z;
    int16_t offset_x;
    int16_t offset_y;
    int16_t offset_z;
};

struct PACKED log_Plane_Nav_Tuning {
    LOG_PACKET_HEADER;
    uint32_t time_ms;
    uint16_t yaw;
    uint32_t wp_distance;
    uint16_t target_bearing_cd;
    uint16_t nav_bearing_cd;
    int16_t altitude_error_cm;
    int16_t airspeed_cm;
    float   altitude;
    uint32_t groundspeed_cm;
};

struct PACKED log_Copter_Compass {
    LOG_PACKET_HEADER;
    int16_t mag_x;
    int16_t mag_y;
    int16_t mag_z;
    int16_t offset_x;
    int16_t offset_y;
    int16_t offset_z;
    int16_t motor_offset_x;
    int16_t motor_offset_y;
    int16_t motor_offset_z;
};

struct PACKED log_Plane_Attitude {
    LOG_PACKET_HEADER;
    uint32_t time_ms;
    int16_t roll;
    int16_t pitch;
    uint16_t yaw;
    uint16_t error_rp;
    uint16_t error_yaw;
};

struct PACKED log_Copter_Attitude {
    LOG_PACKET_HEADER;
    int16_t control_roll;
    int16_t roll;
    int16_t control_pitch;
    int16_t pitch;
    uint16_t control_yaw;
    uint16_t yaw;
};

struct PACKED log_Copter_Control_Tuning {
    LOG_PACKET_HEADER;
    int16_t throttle_in;
    int16_t sonar_alt;
    int32_t baro_alt;
    float   next_wp_alt;
    int16_t desired_sonar_alt;
    int16_t angle_boost;
    int16_t climb_rate;
    int16_t throttle_out;
    int16_t desired_climb_rate;
};

struct PACKED log_Copter_INAV {
    LOG_PACKET_HEADER;
    int16_t baro_alt;
    int16_t inav_alt;
    int16_t inav_climb_rate;
    float   accel_corr_x;
    float   accel_corr_y;
    float   accel_corr_z;
    int32_t gps_lat_from_home;
    int32_t gps_lon_from_home;
    float   inav_lat_from_home;
    float   inav_lon_from_home;
};

void LogReader::process_plane(uint8_t type, uint8_t *data, uint16_t length)
{
    switch (type) {
    case LOG_PLANE_COMPASS_MSG: {
        struct log_Plane_Compass msg;
        if(sizeof(msg) != length) {
            printf("Bad COMPASS length\n");
            exit(1);
        }
        memcpy(&msg, data, sizeof(msg));
        wait_timestamp(msg.time_ms);
        compass.setHIL(Vector3i(msg.mag_x - msg.offset_x, msg.mag_y - msg.offset_y, msg.mag_z - msg.offset_z));
        compass.set_offsets(msg.offset_x, msg.offset_y, msg.offset_z);
        break;
    }

    case LOG_PLANE_ATTITUDE_MSG: {
        struct log_Plane_Attitude msg;
        if(sizeof(msg) != length) {
            printf("Bad ATTITUDE length\n");
            exit(1);
        }
        memcpy(&msg, data, sizeof(msg));
        wait_timestamp(msg.time_ms);
        attitude = Vector3f(msg.roll*0.01f, msg.pitch*0.01f, msg.yaw*0.01f);
        break;
    }

    case LOG_PLANE_NTUN_MSG: {
        struct log_Plane_Nav_Tuning msg;
        if(sizeof(msg) != length) {
            printf("Bad NAV_TUNING length\n");
            exit(1);
        }
        memcpy(&msg, data, sizeof(msg));
        wait_timestamp(msg.time_ms);
        if (ground_alt_cm != 0) {
            baro.setHIL(ground_alt_cm*0.01f + msg.altitude);
        }
        break;
    }
    }
}

void LogReader::process_copter(uint8_t type, uint8_t *data, uint16_t length)
{
    switch (type) {
    case LOG_COPTER_COMPASS_MSG: {
        struct log_Copter_Compass msg;
        if(sizeof(msg) != length) {
            printf("Bad COMPASS length\n");
            exit(1);
        }
        memcpy(&msg, data, sizeof(msg));
        //wait_timestamp(msg.time_ms);
        compass.setHIL(Vector3i(msg.mag_x - msg.offset_x, msg.mag_y - msg.offset_y, msg.mag_z - msg.offset_z));
        compass.set_offsets(msg.offset_x, msg.offset_y, msg.offset_z);
        break;
    }

    case LOG_COPTER_ATTITUDE_MSG: {
        struct log_Copter_Attitude msg;
        if(sizeof(msg) != length) {
            printf("Bad ATTITUDE length\n");
            exit(1);
        }
        memcpy(&msg, data, sizeof(msg));
        //wait_timestamp(msg.time_ms);
        attitude = Vector3f(msg.roll*0.01f, msg.pitch*0.01f, msg.yaw*0.01f);
        break;
    }

    case LOG_COPTER_CONTROL_TUNING_MSG: {
        struct log_Copter_Control_Tuning msg;
        if(sizeof(msg) != length) {
            printf("Bad CONTROL_TUNING length\n");
            exit(1);
        }
        memcpy(&msg, data, sizeof(msg));
        //wait_timestamp(msg.time_ms);
        if (ground_alt_cm != 0) {
            baro.setHIL(0.01f * (ground_alt_cm + msg.baro_alt));
        }
        break;
    }

    case LOG_COPTER_INAV_MSG: {
        struct log_Copter_INAV msg;
        if(sizeof(msg) != length) {
            printf("Bad INAV length\n");
            exit(1);
        }
        memcpy(&msg, data, sizeof(msg));
        inavpos = Vector3f(msg.inav_lat_from_home * LATLON_TO_CM * 0.01f, 
                           msg.inav_lon_from_home * LATLON_TO_CM * 0.01f * 
                           cosf(gps->latitude * 1.0e-7f * DEG_TO_RAD),
                           msg.inav_alt*0.01f);
        break;
    }
    }
}

bool LogReader::update(uint8_t &type)
{
    uint8_t hdr[3];
    if (::read(fd, hdr, 3) != 3) {
        return false;
    }
    if (hdr[0] != HEAD_BYTE1 || hdr[1] != HEAD_BYTE2) {
        printf("bad log header\n");
        return false;
    }

    if (hdr[2] == LOG_FORMAT_MSG) {
        struct log_Format &f = formats[num_formats];
        memcpy(&f, hdr, 3);
        if (::read(fd, &f.type, sizeof(f)-3) != sizeof(f)-3) {
            return false;
        }
        num_formats++;
        type = f.type;
        return true;
    }

    uint8_t i;
    for (i=0; i<num_formats; i++) {
        if (formats[i].type == hdr[2]) break;
    }
    if (i == num_formats) {
        return false;
    }
    const struct log_Format &f = formats[i];
    
    uint8_t data[f.length];
    memcpy(data, hdr, 3);
    if (::read(fd, &data[3], f.length-3) != f.length-3) {
        return false;
    }

    switch (f.type) {
    case LOG_MESSAGE_MSG: {
        struct log_Message msg;
        if(sizeof(msg) != f.length) {
            printf("Bad MESSAGE length\n");
            exit(1);
        }
        memcpy(&msg, data, sizeof(msg));
        if (strncmp(msg.msg, "ArduPlane", strlen("ArduPlane")) == 0) {
            vehicle = VEHICLE_PLANE;
            ::printf("Detected Plane\n");
        } else if (strncmp(msg.msg, "ArduCopter", strlen("ArduCopter")) == 0) {
            vehicle = VEHICLE_COPTER;
            ::printf("Detected Copter\n");
        } else if (strncmp(msg.msg, "APMRover2", strlen("APMRover2")) == 0) {
            vehicle = VEHICLE_ROVER;
            ::printf("Detected Rover\n");
        }
        break;
    }

    case LOG_IMU_MSG: {
        struct log_IMU msg;
        if(sizeof(msg) != f.length) {
            printf("Bad IMU length\n");
            exit(1);
        }
        memcpy(&msg, data, sizeof(msg));
        wait_timestamp(msg.timestamp);
        ins.set_gyro(Vector3f(msg.gyro_x, msg.gyro_y, msg.gyro_z));
        ins.set_accel(Vector3f(msg.accel_x, msg.accel_y, msg.accel_z));
        break;
    }

    case LOG_GPS_MSG: {
        struct log_GPS msg;
        if(sizeof(msg) != f.length) {
            printf("Bad GPS length\n");
            exit(1);
        }
        memcpy(&msg, data, sizeof(msg));
        wait_timestamp(msg.apm_time);
        gps->setHIL(msg.apm_time,
                    msg.latitude*1.0e-7f, 
                    msg.longitude*1.0e-7f, 
                    msg.altitude*1.0e-2f,
                    msg.ground_speed*1.0e-2f, 
                    msg.ground_course*1.0e-2f, 
                    0, 
                    msg.status==3?msg.num_sats:0);
        if (msg.status == 3 && ground_alt_cm == 0) {
            ground_alt_cm = msg.altitude;
        }
        break;
    }

    case LOG_SIMSTATE_MSG: {
        struct log_AHRS msg;
        if(sizeof(msg) != f.length) {
            printf("Bad SIMSTATE length\n");
            exit(1);
        }
        memcpy(&msg, data, sizeof(msg));
        wait_timestamp(msg.time_ms);
        sim_attitude = Vector3f(msg.roll*0.01f, msg.pitch*0.01f, msg.yaw*0.01f);
        break;
    }

    default:
        if (vehicle == VEHICLE_PLANE) {
            process_plane(f.type, data, f.length);
        } else if (vehicle == VEHICLE_COPTER) {
            process_copter(f.type, data, f.length);
        }
        break;
    }

    type = f.type;

    return true;
}

void LogReader::wait_timestamp(uint32_t timestamp)
{
    hal.scheduler->stop_clock(timestamp);
}

bool LogReader::wait_type(uint8_t wtype)
{
    while (true) {
        uint8_t type;
        if (!update(type)) {
            return false;
        }
        if (wtype == type) {
            break;
        }
    }
    return true;
}
