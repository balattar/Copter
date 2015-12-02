// MESSAGE REMOTE_LOG_DATA_BLOCK PACKING

#define MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK 184

typedef struct __mavlink_remote_log_data_block_t
{
 uint32_t seqno; /*< log data block sequence number*/
 uint8_t target_system; /*< System ID*/
 uint8_t target_component; /*< Component ID*/
 uint8_t data[200]; /*< log data block*/
} mavlink_remote_log_data_block_t;

#define MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN 206
#define MAVLINK_MSG_ID_184_LEN 206

#define MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_CRC 159
#define MAVLINK_MSG_ID_184_CRC 159

#define MAVLINK_MSG_REMOTE_LOG_DATA_BLOCK_FIELD_DATA_LEN 200

#define MAVLINK_MESSAGE_INFO_REMOTE_LOG_DATA_BLOCK { \
	"REMOTE_LOG_DATA_BLOCK", \
	4, \
	{  { "seqno", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_remote_log_data_block_t, seqno) }, \
         { "target_system", NULL, MAVLINK_TYPE_UINT8_T, 0, 4, offsetof(mavlink_remote_log_data_block_t, target_system) }, \
         { "target_component", NULL, MAVLINK_TYPE_UINT8_T, 0, 5, offsetof(mavlink_remote_log_data_block_t, target_component) }, \
         { "data", NULL, MAVLINK_TYPE_UINT8_T, 200, 6, offsetof(mavlink_remote_log_data_block_t, data) }, \
         } \
}


/**
 * @brief Pack a remote_log_data_block message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param target_system System ID
 * @param target_component Component ID
 * @param seqno log data block sequence number
 * @param data log data block
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_remote_log_data_block_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint8_t target_system, uint8_t target_component, uint32_t seqno, const uint8_t *data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN];
	_mav_put_uint32_t(buf, 0, seqno);
	_mav_put_uint8_t(buf, 4, target_system);
	_mav_put_uint8_t(buf, 5, target_component);
	_mav_put_uint8_t_array(buf, 6, data, 200);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN);
#else
	mavlink_remote_log_data_block_t packet;
	packet.seqno = seqno;
	packet.target_system = target_system;
	packet.target_component = target_component;
	mav_array_memcpy(packet.data, data, sizeof(uint8_t)*200);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN);
#endif
}

/**
 * @brief Pack a remote_log_data_block message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param target_system System ID
 * @param target_component Component ID
 * @param seqno log data block sequence number
 * @param data log data block
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_remote_log_data_block_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint8_t target_system,uint8_t target_component,uint32_t seqno,const uint8_t *data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN];
	_mav_put_uint32_t(buf, 0, seqno);
	_mav_put_uint8_t(buf, 4, target_system);
	_mav_put_uint8_t(buf, 5, target_component);
	_mav_put_uint8_t_array(buf, 6, data, 200);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN);
#else
	mavlink_remote_log_data_block_t packet;
	packet.seqno = seqno;
	packet.target_system = target_system;
	packet.target_component = target_component;
	mav_array_memcpy(packet.data, data, sizeof(uint8_t)*200);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN);
#endif
}

/**
 * @brief Encode a remote_log_data_block struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param remote_log_data_block C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_remote_log_data_block_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_remote_log_data_block_t* remote_log_data_block)
{
	return mavlink_msg_remote_log_data_block_pack(system_id, component_id, msg, remote_log_data_block->target_system, remote_log_data_block->target_component, remote_log_data_block->seqno, remote_log_data_block->data);
}

/**
 * @brief Encode a remote_log_data_block struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param remote_log_data_block C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_remote_log_data_block_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_remote_log_data_block_t* remote_log_data_block)
{
	return mavlink_msg_remote_log_data_block_pack_chan(system_id, component_id, chan, msg, remote_log_data_block->target_system, remote_log_data_block->target_component, remote_log_data_block->seqno, remote_log_data_block->data);
}

/**
 * @brief Send a remote_log_data_block message
 * @param chan MAVLink channel to send the message
 *
 * @param target_system System ID
 * @param target_component Component ID
 * @param seqno log data block sequence number
 * @param data log data block
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_remote_log_data_block_send(mavlink_channel_t chan, uint8_t target_system, uint8_t target_component, uint32_t seqno, const uint8_t *data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN];
	_mav_put_uint32_t(buf, 0, seqno);
	_mav_put_uint8_t(buf, 4, target_system);
	_mav_put_uint8_t(buf, 5, target_component);
	_mav_put_uint8_t_array(buf, 6, data, 200);
#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK, buf, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK, buf, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN);
#endif
#else
	mavlink_remote_log_data_block_t packet;
	packet.seqno = seqno;
	packet.target_system = target_system;
	packet.target_component = target_component;
	mav_array_memcpy(packet.data, data, sizeof(uint8_t)*200);
#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK, (const char *)&packet, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK, (const char *)&packet, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_remote_log_data_block_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t target_system, uint8_t target_component, uint32_t seqno, const uint8_t *data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint32_t(buf, 0, seqno);
	_mav_put_uint8_t(buf, 4, target_system);
	_mav_put_uint8_t(buf, 5, target_component);
	_mav_put_uint8_t_array(buf, 6, data, 200);
#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK, buf, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK, buf, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN);
#endif
#else
	mavlink_remote_log_data_block_t *packet = (mavlink_remote_log_data_block_t *)msgbuf;
	packet->seqno = seqno;
	packet->target_system = target_system;
	packet->target_component = target_component;
	mav_array_memcpy(packet->data, data, sizeof(uint8_t)*200);
#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK, (const char *)packet, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK, (const char *)packet, MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE REMOTE_LOG_DATA_BLOCK UNPACKING


/**
 * @brief Get field target_system from remote_log_data_block message
 *
 * @return System ID
 */
static inline uint8_t mavlink_msg_remote_log_data_block_get_target_system(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  4);
}

/**
 * @brief Get field target_component from remote_log_data_block message
 *
 * @return Component ID
 */
static inline uint8_t mavlink_msg_remote_log_data_block_get_target_component(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  5);
}

/**
 * @brief Get field seqno from remote_log_data_block message
 *
 * @return log data block sequence number
 */
static inline uint32_t mavlink_msg_remote_log_data_block_get_seqno(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint32_t(msg,  0);
}

/**
 * @brief Get field data from remote_log_data_block message
 *
 * @return log data block
 */
static inline uint16_t mavlink_msg_remote_log_data_block_get_data(const mavlink_message_t* msg, uint8_t *data)
{
	return _MAV_RETURN_uint8_t_array(msg, data, 200,  6);
}

/**
 * @brief Decode a remote_log_data_block message into a struct
 *
 * @param msg The message to decode
 * @param remote_log_data_block C-struct to decode the message contents into
 */
static inline void mavlink_msg_remote_log_data_block_decode(const mavlink_message_t* msg, mavlink_remote_log_data_block_t* remote_log_data_block)
{
#if MAVLINK_NEED_BYTE_SWAP
	remote_log_data_block->seqno = mavlink_msg_remote_log_data_block_get_seqno(msg);
	remote_log_data_block->target_system = mavlink_msg_remote_log_data_block_get_target_system(msg);
	remote_log_data_block->target_component = mavlink_msg_remote_log_data_block_get_target_component(msg);
	mavlink_msg_remote_log_data_block_get_data(msg, remote_log_data_block->data);
#else
	memcpy(remote_log_data_block, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK_LEN);
#endif
}
