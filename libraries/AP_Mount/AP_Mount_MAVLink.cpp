// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <AP_Mount_MAVLink.h>
#if AP_AHRS_NAVEKF_AVAILABLE
#include <GCS_MAVLink.h>

#define MOUNT_DEBUG 0

#if MOUNT_DEBUG
#include <stdio.h>
#endif

AP_Mount_MAVLink::AP_Mount_MAVLink(AP_Mount &frontend, AP_Mount::mount_state state, uint8_t instance) :
    AP_Mount_Backend(frontend, state, instance),
    _initialised(false),
    _chan(MAVLINK_COMM_0),
    _last_mode(MAV_MOUNT_MODE_RETRACT),
    _ekf(frontend._ahrs)
{}

// init - performs any required initialisation for this instance
void AP_Mount_MAVLink::init(const AP_SerialManager& serial_manager)
{
    // use mavlink channel associated with MAVLink2 protocol
    if (serial_manager.get_mavlink_channel(AP_SerialManager::SerialProtocol_MAVLink2, _chan)) {
        _initialised = true;
    }
}

// update mount position - should be called periodically
void AP_Mount_MAVLink::update()
{
    // exit immediately if not initialised
    if (!_initialised) {
        return;
    }

    // update based on mount mode
    switch(get_mode()) {
        // move mount to a "retracted" position.  we do not implement a separate servo based retract mechanism
        case MAV_MOUNT_MODE_RETRACT:
            send_angle_target(_state._retract_angles.get(), true);
            break;

        // move mount to a neutral position, typically pointing forward
        case MAV_MOUNT_MODE_NEUTRAL:
            send_angle_target(_state._neutral_angles.get(), true);
            break;

        // point to the angles given by a mavlink message
        case MAV_MOUNT_MODE_MAVLINK_TARGETING:
            // do nothing because earth-frame angle targets (i.e. _angle_ef_target_rad) should have already been set by a MOUNT_CONTROL message from GCS
            break;

        // RC radio manual angle control, but with stabilization from the AHRS
        case MAV_MOUNT_MODE_RC_TARGETING:
            // update targets using pilot's rc inputs
            update_targets_from_rc();
            send_angle_target(_angle_ef_target_rad, false);
            break;

        // point mount to a GPS point given by the mission planner
        case MAV_MOUNT_MODE_GPS_POINT:
            if(_frontend._ahrs.get_gps().status() >= AP_GPS::GPS_OK_FIX_2D) {
                calc_angle_to_location(_state._roi_target, _angle_ef_target_rad, true, false);
                send_angle_target(_angle_ef_target_rad, false);
            }
            break;

        default:
            // we do not know this mode so do nothing
            break;
    }
}

// has_pan_control - returns true if this mount can control it's pan (required for multicopters)
bool AP_Mount_MAVLink::has_pan_control() const
{
    // we do not have yaw control
    return false;
}

// set_mode - sets mount's mode
void AP_Mount_MAVLink::set_mode(enum MAV_MOUNT_MODE mode)
{
    // exit immediately if not initialised
    if (!_initialised) {
        return;
    }

    // map requested mode to mode that mount can actually support
    enum MAV_MOUNT_MODE mode_to_send = mode;
    switch (mode) {
        case MAV_MOUNT_MODE_RETRACT:
        case MAV_MOUNT_MODE_NEUTRAL:
        case MAV_MOUNT_MODE_MAVLINK_TARGETING:
        case MAV_MOUNT_MODE_RC_TARGETING:
        case MAV_MOUNT_MODE_GPS_POINT:
            mode_to_send = MAV_MOUNT_MODE_MAVLINK_TARGETING;
            break;
        default:
            // unknown mode so just send it and hopefully gimbal supports it
            break;
    }

    // prepare and send command_long message with DO_SET_MODE command
    mavlink_msg_command_long_send(
            _chan, mavlink_system.sysid, AP_MOUNT_MAVLINK_COMPID, // channel, system id, component id
            MAV_CMD_DO_SET_MODE,    // command number
            0,  // confirmation: 0=first confirmation of this command
            mode_to_send,   // param1: mode
            0,      // param2: custom mode
            0.0f, 0.0f, 0.0f,0.0f, 0.0f);    // param3 ~ param 7: not used

    // record the mode change
    _state._mode = mode;
    _last_mode = mode_to_send;
}

// status_msg - called to allow mounts to send their status to GCS using the MOUNT_STATUS message
void AP_Mount_MAVLink::status_msg(mavlink_channel_t chan)
{
    // do nothing - we rely on the mount sending the messages directly
}

// send_angle_target - send earth-frame angle targets to mount
void AP_Mount_MAVLink::send_angle_target(const Vector3f& target, bool target_in_degrees)
{
    // exit immediately if not initialised
    if (!_initialised) {
        return;
    }

    // convert to degrees if necessary
    Vector3f target_deg = target;
    if (!target_in_degrees) {
        target_deg *= RAD_TO_DEG;
    }

    // exit immediately if mode and targets have not changed since last time they were sent
    if (_state._mode == MAV_MOUNT_MODE_MAVLINK_TARGETING && target_deg == _last_angle_target) {
        return;
    }

    // prepare and send command_long message with DO_MOUNT_CONTROL command
    mavlink_msg_command_long_send(
            _chan, mavlink_system.sysid, AP_MOUNT_MAVLINK_COMPID, // channel, system id, component id
            MAV_CMD_DO_MOUNT_CONTROL,   // command number
            0,  // confirmation: 0=first confirmation of this command
            target_deg.y,   // param1: pitch (in degrees) or lat (as int32_t)
            target_deg.x,   // param2: roll (in degrees) or lon (as int32_t)
            target_deg.z,   // param3: yaw (in degrees) or alt (in meters).
            0.0f,0.0f,0.0f,                     // param4 ~ param6 : not used
            MAV_MOUNT_MODE_MAVLINK_TARGETING);  // param7: MAV_MOUNT_MODE enum value

    // store sent target and mode
    _last_angle_target = target_deg;
    _last_mode = MAV_MOUNT_MODE_MAVLINK_TARGETING;
}

/*
  handle a GIMBAL_REPORT message
 */
void AP_Mount_MAVLink::handle_gimbal_report(mavlink_channel_t chan, mavlink_message_t *msg)
{
    // just save it for future processing and reporting to GCS for now
    mavlink_msg_gimbal_report_decode(msg, &_gimbal_report);

    Vector3f delta_angles(_gimbal_report.delta_angle_x,
                          _gimbal_report.delta_angle_y,
                          _gimbal_report.delta_angle_z);
    Vector3f delta_velocity(_gimbal_report.delta_velocity_x,
                            _gimbal_report.delta_velocity_y,
                            _gimbal_report.delta_velocity_z);
    Vector3f joint_angles(_gimbal_report.joint_roll,
                          _gimbal_report.joint_pitch,
                          _gimbal_report.joint_yaw);
    _ekf.RunEKF(_gimbal_report.delta_time, delta_angles, delta_velocity, joint_angles);

    // get the gyro bias data
    Vector3f gyroBias;
    _ekf.getGyroBias(gyroBias);

    // get the gimbal estimated quaternion
    Quaternion quatEst;
    _ekf.getQuat(quatEst);

    // set the demanded quaternion - tilt down with a roll and yaw of zero
    Quaternion quatDem;
    quatDem[0] =  sqrtf(0.75f);
    quatDem[2] = -sqrtf(0.25f);

    //divide the demanded quaternion by the estimated to get the error
    Quaternion quatErr = quatDem / quatEst;

    // convert the quaternion to an angle error vector
    Vector3f deltaAngErr;
    float scaler = 1.0f-quatErr[0]*quatErr[0];
    if (scaler > 1e-12) {
        scaler = 1.0f/sqrtf(scaler);
        deltaAngErr.x = quatErr[1] * scaler;
        deltaAngErr.y = quatErr[2] * scaler;
        deltaAngErr.z = quatErr[3] * scaler;
    } else {
        deltaAngErr.zero();
    }

    // multiply the angle error vector by a gain to calculate a demanded gimbal rate
    Vector3f rateDemand = deltaAngErr * 1.0f;

    // Constrain the demanded rate to a length of 0.5 rad /sec
    float length = rateDemand.length();
    if (length > 0.5f) {
        rateDemand = rateDemand * (0.5f / length);
    }

    // send the gimbal control message
    mavlink_msg_gimbal_control_send(chan, 
                                    msg->sysid,
                                    msg->compid,
                                    rateDemand.x, rateDemand.y, rateDemand.z, // demanded rates
                                    gyroBias.x, gyroBias.y, gyroBias.z);
}

/*
  send a GIMBAL_REPORT message to the GCS
 */
void AP_Mount_MAVLink::send_gimbal_report(mavlink_channel_t chan)
{
    mavlink_msg_gimbal_report_send(chan, 
                                   0, 0, // send as broadcast
                                   _gimbal_report.delta_time, 
                                   _gimbal_report.delta_angle_x, 
                                   _gimbal_report.delta_angle_y, 
                                   _gimbal_report.delta_angle_z, 
                                   _gimbal_report.delta_velocity_x, 
                                   _gimbal_report.delta_velocity_y, 
                                   _gimbal_report.delta_velocity_z, 
                                   _gimbal_report.joint_roll, 
                                   _gimbal_report.joint_pitch, 
                                   _gimbal_report.joint_yaw);
    float tilt;
    Vector3f velocity, euler, gyroBias;
    _ekf.getDebug(tilt, velocity, euler, gyroBias);
#if MOUNT_DEBUG
    ::printf("tilt=%.2f euler=(%.2f, %.2f, %.2f) vel=(%.2f, %.2f %.2f)\n",
             tilt,
             degrees(euler.x), degrees(euler.y), degrees(euler.z),
             (velocity.x), (velocity.y), (velocity.z));
#endif
}

#endif // AP_AHRS_NAVEKF_AVAILABLE
