// MESSAGE GIMBAL_REPORT PACKING

#define MAVLINK_MSG_ID_GIMBAL_REPORT 184

typedef struct __mavlink_gimbal_report_t
{
 uint32_t counter; ///< Report counter
 float delta_angle_x; ///< Delta angle X, radians
 float delta_angle_y; ///< Delta angle Y, radians
 float delta_angle_z; ///< Delta angle Z, radians
 float delta_velocity_x; ///< Delta velocity X, m/s
 float delta_velocity_y; ///< Delta velocity Y, m/s
 float delta_velocity_z; ///< Delta velocity Z, m/s
 float joint_roll; ///< Joint roll, radians
 float joint_pitch; ///< Joint pitch, radians
 float joint_yaw; ///< Joint yaw, radians
} mavlink_gimbal_report_t;

#define MAVLINK_MSG_ID_GIMBAL_REPORT_LEN 40
#define MAVLINK_MSG_ID_184_LEN 40

#define MAVLINK_MSG_ID_GIMBAL_REPORT_CRC 160
#define MAVLINK_MSG_ID_184_CRC 160



#define MAVLINK_MESSAGE_INFO_GIMBAL_REPORT { \
	"GIMBAL_REPORT", \
	10, \
	{  { "counter", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_gimbal_report_t, counter) }, \
         { "delta_angle_x", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_gimbal_report_t, delta_angle_x) }, \
         { "delta_angle_y", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_gimbal_report_t, delta_angle_y) }, \
         { "delta_angle_z", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_gimbal_report_t, delta_angle_z) }, \
         { "delta_velocity_x", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_gimbal_report_t, delta_velocity_x) }, \
         { "delta_velocity_y", NULL, MAVLINK_TYPE_FLOAT, 0, 20, offsetof(mavlink_gimbal_report_t, delta_velocity_y) }, \
         { "delta_velocity_z", NULL, MAVLINK_TYPE_FLOAT, 0, 24, offsetof(mavlink_gimbal_report_t, delta_velocity_z) }, \
         { "joint_roll", NULL, MAVLINK_TYPE_FLOAT, 0, 28, offsetof(mavlink_gimbal_report_t, joint_roll) }, \
         { "joint_pitch", NULL, MAVLINK_TYPE_FLOAT, 0, 32, offsetof(mavlink_gimbal_report_t, joint_pitch) }, \
         { "joint_yaw", NULL, MAVLINK_TYPE_FLOAT, 0, 36, offsetof(mavlink_gimbal_report_t, joint_yaw) }, \
         } \
}


/**
 * @brief Pack a gimbal_report message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param counter Report counter
 * @param delta_angle_x Delta angle X, radians
 * @param delta_angle_y Delta angle Y, radians
 * @param delta_angle_z Delta angle Z, radians
 * @param delta_velocity_x Delta velocity X, m/s
 * @param delta_velocity_y Delta velocity Y, m/s
 * @param delta_velocity_z Delta velocity Z, m/s
 * @param joint_roll Joint roll, radians
 * @param joint_pitch Joint pitch, radians
 * @param joint_yaw Joint yaw, radians
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_gimbal_report_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint32_t counter, float delta_angle_x, float delta_angle_y, float delta_angle_z, float delta_velocity_x, float delta_velocity_y, float delta_velocity_z, float joint_roll, float joint_pitch, float joint_yaw)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_GIMBAL_REPORT_LEN];
	_mav_put_uint32_t(buf, 0, counter);
	_mav_put_float(buf, 4, delta_angle_x);
	_mav_put_float(buf, 8, delta_angle_y);
	_mav_put_float(buf, 12, delta_angle_z);
	_mav_put_float(buf, 16, delta_velocity_x);
	_mav_put_float(buf, 20, delta_velocity_y);
	_mav_put_float(buf, 24, delta_velocity_z);
	_mav_put_float(buf, 28, joint_roll);
	_mav_put_float(buf, 32, joint_pitch);
	_mav_put_float(buf, 36, joint_yaw);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_GIMBAL_REPORT_LEN);
#else
	mavlink_gimbal_report_t packet;
	packet.counter = counter;
	packet.delta_angle_x = delta_angle_x;
	packet.delta_angle_y = delta_angle_y;
	packet.delta_angle_z = delta_angle_z;
	packet.delta_velocity_x = delta_velocity_x;
	packet.delta_velocity_y = delta_velocity_y;
	packet.delta_velocity_z = delta_velocity_z;
	packet.joint_roll = joint_roll;
	packet.joint_pitch = joint_pitch;
	packet.joint_yaw = joint_yaw;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_GIMBAL_REPORT_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_GIMBAL_REPORT;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_GIMBAL_REPORT_LEN, MAVLINK_MSG_ID_GIMBAL_REPORT_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_GIMBAL_REPORT_LEN);
#endif
}

/**
 * @brief Pack a gimbal_report message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param counter Report counter
 * @param delta_angle_x Delta angle X, radians
 * @param delta_angle_y Delta angle Y, radians
 * @param delta_angle_z Delta angle Z, radians
 * @param delta_velocity_x Delta velocity X, m/s
 * @param delta_velocity_y Delta velocity Y, m/s
 * @param delta_velocity_z Delta velocity Z, m/s
 * @param joint_roll Joint roll, radians
 * @param joint_pitch Joint pitch, radians
 * @param joint_yaw Joint yaw, radians
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_gimbal_report_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint32_t counter,float delta_angle_x,float delta_angle_y,float delta_angle_z,float delta_velocity_x,float delta_velocity_y,float delta_velocity_z,float joint_roll,float joint_pitch,float joint_yaw)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_GIMBAL_REPORT_LEN];
	_mav_put_uint32_t(buf, 0, counter);
	_mav_put_float(buf, 4, delta_angle_x);
	_mav_put_float(buf, 8, delta_angle_y);
	_mav_put_float(buf, 12, delta_angle_z);
	_mav_put_float(buf, 16, delta_velocity_x);
	_mav_put_float(buf, 20, delta_velocity_y);
	_mav_put_float(buf, 24, delta_velocity_z);
	_mav_put_float(buf, 28, joint_roll);
	_mav_put_float(buf, 32, joint_pitch);
	_mav_put_float(buf, 36, joint_yaw);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_GIMBAL_REPORT_LEN);
#else
	mavlink_gimbal_report_t packet;
	packet.counter = counter;
	packet.delta_angle_x = delta_angle_x;
	packet.delta_angle_y = delta_angle_y;
	packet.delta_angle_z = delta_angle_z;
	packet.delta_velocity_x = delta_velocity_x;
	packet.delta_velocity_y = delta_velocity_y;
	packet.delta_velocity_z = delta_velocity_z;
	packet.joint_roll = joint_roll;
	packet.joint_pitch = joint_pitch;
	packet.joint_yaw = joint_yaw;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_GIMBAL_REPORT_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_GIMBAL_REPORT;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_GIMBAL_REPORT_LEN, MAVLINK_MSG_ID_GIMBAL_REPORT_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_GIMBAL_REPORT_LEN);
#endif
}

/**
 * @brief Encode a gimbal_report struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param gimbal_report C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_gimbal_report_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_gimbal_report_t* gimbal_report)
{
	return mavlink_msg_gimbal_report_pack(system_id, component_id, msg, gimbal_report->counter, gimbal_report->delta_angle_x, gimbal_report->delta_angle_y, gimbal_report->delta_angle_z, gimbal_report->delta_velocity_x, gimbal_report->delta_velocity_y, gimbal_report->delta_velocity_z, gimbal_report->joint_roll, gimbal_report->joint_pitch, gimbal_report->joint_yaw);
}

/**
 * @brief Encode a gimbal_report struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param gimbal_report C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_gimbal_report_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_gimbal_report_t* gimbal_report)
{
	return mavlink_msg_gimbal_report_pack_chan(system_id, component_id, chan, msg, gimbal_report->counter, gimbal_report->delta_angle_x, gimbal_report->delta_angle_y, gimbal_report->delta_angle_z, gimbal_report->delta_velocity_x, gimbal_report->delta_velocity_y, gimbal_report->delta_velocity_z, gimbal_report->joint_roll, gimbal_report->joint_pitch, gimbal_report->joint_yaw);
}

/**
 * @brief Send a gimbal_report message
 * @param chan MAVLink channel to send the message
 *
 * @param counter Report counter
 * @param delta_angle_x Delta angle X, radians
 * @param delta_angle_y Delta angle Y, radians
 * @param delta_angle_z Delta angle Z, radians
 * @param delta_velocity_x Delta velocity X, m/s
 * @param delta_velocity_y Delta velocity Y, m/s
 * @param delta_velocity_z Delta velocity Z, m/s
 * @param joint_roll Joint roll, radians
 * @param joint_pitch Joint pitch, radians
 * @param joint_yaw Joint yaw, radians
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_gimbal_report_send(mavlink_channel_t chan, uint32_t counter, float delta_angle_x, float delta_angle_y, float delta_angle_z, float delta_velocity_x, float delta_velocity_y, float delta_velocity_z, float joint_roll, float joint_pitch, float joint_yaw)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_GIMBAL_REPORT_LEN];
	_mav_put_uint32_t(buf, 0, counter);
	_mav_put_float(buf, 4, delta_angle_x);
	_mav_put_float(buf, 8, delta_angle_y);
	_mav_put_float(buf, 12, delta_angle_z);
	_mav_put_float(buf, 16, delta_velocity_x);
	_mav_put_float(buf, 20, delta_velocity_y);
	_mav_put_float(buf, 24, delta_velocity_z);
	_mav_put_float(buf, 28, joint_roll);
	_mav_put_float(buf, 32, joint_pitch);
	_mav_put_float(buf, 36, joint_yaw);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GIMBAL_REPORT, buf, MAVLINK_MSG_ID_GIMBAL_REPORT_LEN, MAVLINK_MSG_ID_GIMBAL_REPORT_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GIMBAL_REPORT, buf, MAVLINK_MSG_ID_GIMBAL_REPORT_LEN);
#endif
#else
	mavlink_gimbal_report_t packet;
	packet.counter = counter;
	packet.delta_angle_x = delta_angle_x;
	packet.delta_angle_y = delta_angle_y;
	packet.delta_angle_z = delta_angle_z;
	packet.delta_velocity_x = delta_velocity_x;
	packet.delta_velocity_y = delta_velocity_y;
	packet.delta_velocity_z = delta_velocity_z;
	packet.joint_roll = joint_roll;
	packet.joint_pitch = joint_pitch;
	packet.joint_yaw = joint_yaw;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GIMBAL_REPORT, (const char *)&packet, MAVLINK_MSG_ID_GIMBAL_REPORT_LEN, MAVLINK_MSG_ID_GIMBAL_REPORT_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GIMBAL_REPORT, (const char *)&packet, MAVLINK_MSG_ID_GIMBAL_REPORT_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_GIMBAL_REPORT_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_gimbal_report_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint32_t counter, float delta_angle_x, float delta_angle_y, float delta_angle_z, float delta_velocity_x, float delta_velocity_y, float delta_velocity_z, float joint_roll, float joint_pitch, float joint_yaw)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint32_t(buf, 0, counter);
	_mav_put_float(buf, 4, delta_angle_x);
	_mav_put_float(buf, 8, delta_angle_y);
	_mav_put_float(buf, 12, delta_angle_z);
	_mav_put_float(buf, 16, delta_velocity_x);
	_mav_put_float(buf, 20, delta_velocity_y);
	_mav_put_float(buf, 24, delta_velocity_z);
	_mav_put_float(buf, 28, joint_roll);
	_mav_put_float(buf, 32, joint_pitch);
	_mav_put_float(buf, 36, joint_yaw);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GIMBAL_REPORT, buf, MAVLINK_MSG_ID_GIMBAL_REPORT_LEN, MAVLINK_MSG_ID_GIMBAL_REPORT_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GIMBAL_REPORT, buf, MAVLINK_MSG_ID_GIMBAL_REPORT_LEN);
#endif
#else
	mavlink_gimbal_report_t *packet = (mavlink_gimbal_report_t *)msgbuf;
	packet->counter = counter;
	packet->delta_angle_x = delta_angle_x;
	packet->delta_angle_y = delta_angle_y;
	packet->delta_angle_z = delta_angle_z;
	packet->delta_velocity_x = delta_velocity_x;
	packet->delta_velocity_y = delta_velocity_y;
	packet->delta_velocity_z = delta_velocity_z;
	packet->joint_roll = joint_roll;
	packet->joint_pitch = joint_pitch;
	packet->joint_yaw = joint_yaw;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GIMBAL_REPORT, (const char *)packet, MAVLINK_MSG_ID_GIMBAL_REPORT_LEN, MAVLINK_MSG_ID_GIMBAL_REPORT_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GIMBAL_REPORT, (const char *)packet, MAVLINK_MSG_ID_GIMBAL_REPORT_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE GIMBAL_REPORT UNPACKING


/**
 * @brief Get field counter from gimbal_report message
 *
 * @return Report counter
 */
static inline uint32_t mavlink_msg_gimbal_report_get_counter(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint32_t(msg,  0);
}

/**
 * @brief Get field delta_angle_x from gimbal_report message
 *
 * @return Delta angle X, radians
 */
static inline float mavlink_msg_gimbal_report_get_delta_angle_x(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  4);
}

/**
 * @brief Get field delta_angle_y from gimbal_report message
 *
 * @return Delta angle Y, radians
 */
static inline float mavlink_msg_gimbal_report_get_delta_angle_y(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  8);
}

/**
 * @brief Get field delta_angle_z from gimbal_report message
 *
 * @return Delta angle Z, radians
 */
static inline float mavlink_msg_gimbal_report_get_delta_angle_z(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  12);
}

/**
 * @brief Get field delta_velocity_x from gimbal_report message
 *
 * @return Delta velocity X, m/s
 */
static inline float mavlink_msg_gimbal_report_get_delta_velocity_x(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  16);
}

/**
 * @brief Get field delta_velocity_y from gimbal_report message
 *
 * @return Delta velocity Y, m/s
 */
static inline float mavlink_msg_gimbal_report_get_delta_velocity_y(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  20);
}

/**
 * @brief Get field delta_velocity_z from gimbal_report message
 *
 * @return Delta velocity Z, m/s
 */
static inline float mavlink_msg_gimbal_report_get_delta_velocity_z(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  24);
}

/**
 * @brief Get field joint_roll from gimbal_report message
 *
 * @return Joint roll, radians
 */
static inline float mavlink_msg_gimbal_report_get_joint_roll(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  28);
}

/**
 * @brief Get field joint_pitch from gimbal_report message
 *
 * @return Joint pitch, radians
 */
static inline float mavlink_msg_gimbal_report_get_joint_pitch(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  32);
}

/**
 * @brief Get field joint_yaw from gimbal_report message
 *
 * @return Joint yaw, radians
 */
static inline float mavlink_msg_gimbal_report_get_joint_yaw(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  36);
}

/**
 * @brief Decode a gimbal_report message into a struct
 *
 * @param msg The message to decode
 * @param gimbal_report C-struct to decode the message contents into
 */
static inline void mavlink_msg_gimbal_report_decode(const mavlink_message_t* msg, mavlink_gimbal_report_t* gimbal_report)
{
#if MAVLINK_NEED_BYTE_SWAP
	gimbal_report->counter = mavlink_msg_gimbal_report_get_counter(msg);
	gimbal_report->delta_angle_x = mavlink_msg_gimbal_report_get_delta_angle_x(msg);
	gimbal_report->delta_angle_y = mavlink_msg_gimbal_report_get_delta_angle_y(msg);
	gimbal_report->delta_angle_z = mavlink_msg_gimbal_report_get_delta_angle_z(msg);
	gimbal_report->delta_velocity_x = mavlink_msg_gimbal_report_get_delta_velocity_x(msg);
	gimbal_report->delta_velocity_y = mavlink_msg_gimbal_report_get_delta_velocity_y(msg);
	gimbal_report->delta_velocity_z = mavlink_msg_gimbal_report_get_delta_velocity_z(msg);
	gimbal_report->joint_roll = mavlink_msg_gimbal_report_get_joint_roll(msg);
	gimbal_report->joint_pitch = mavlink_msg_gimbal_report_get_joint_pitch(msg);
	gimbal_report->joint_yaw = mavlink_msg_gimbal_report_get_joint_yaw(msg);
#else
	memcpy(gimbal_report, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_GIMBAL_REPORT_LEN);
#endif
}
