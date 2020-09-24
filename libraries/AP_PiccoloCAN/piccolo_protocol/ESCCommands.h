// ESCCommands.h was generated by ProtoGen version 3.2.a

/*
 * Copyright Currawong Engineering Pty Ltd
 * www.currawongeng.com
 * all rights reserved
 */
 

#ifndef _ESCCOMMANDS_H
#define _ESCCOMMANDS_H

// Language target is C, C++ compilers: don't mangle us
#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \file
 */

#include <stdbool.h>
#include "ESCVelocityProtocol.h"

/*!
 * Set the CAN Node ID of the target ESC
 */
typedef struct
{
    uint32_t serialNumber; //!< The serial number must match that of the target ESC for the command to be accepted
    uint8_t  nodeID;       //!< The new Node ID of the ESC
}ESC_SetNodeID_t;

//! Create the ESC_SetNodeID packet from parameters
void encodeESC_SetNodeIDPacket(void* pkt, uint32_t serialNumber, uint8_t nodeID);

//! Decode the ESC_SetNodeID packet to parameters
int decodeESC_SetNodeIDPacket(const void* pkt, uint32_t* serialNumber, uint8_t* nodeID);

//! return the packet ID for the ESC_SetNodeID packet
#define getESC_SetNodeIDPacketID() (PKT_ESC_SYSTEM_CMD)

//! return the minimum encoded length for the ESC_SetNodeID packet
#define getESC_SetNodeIDMinDataLength() (6)

//! return the maximum encoded length for the ESC_SetNodeID packet
#define getESC_SetNodeIDMaxDataLength() (6)

//! Create the ESC_SetUserIDA packet from parameters
void encodeESC_SetUserIDAPacket(void* pkt, uint16_t id);

//! Decode the ESC_SetUserIDA packet to parameters
int decodeESC_SetUserIDAPacket(const void* pkt, uint16_t* id);

//! return the packet ID for the ESC_SetUserIDA packet
#define getESC_SetUserIDAPacketID() (PKT_ESC_SYSTEM_CMD)

//! return the minimum encoded length for the ESC_SetUserIDA packet
#define getESC_SetUserIDAMinDataLength() (3)

//! return the maximum encoded length for the ESC_SetUserIDA packet
#define getESC_SetUserIDAMaxDataLength() (3)

//! Create the ESC_SetUserIDB packet from parameters
void encodeESC_SetUserIDBPacket(void* pkt, uint16_t id);

//! Decode the ESC_SetUserIDB packet to parameters
int decodeESC_SetUserIDBPacket(const void* pkt, uint16_t* id);

//! return the packet ID for the ESC_SetUserIDB packet
#define getESC_SetUserIDBPacketID() (PKT_ESC_SYSTEM_CMD)

//! return the minimum encoded length for the ESC_SetUserIDB packet
#define getESC_SetUserIDBMinDataLength() (3)

//! return the maximum encoded length for the ESC_SetUserIDB packet
#define getESC_SetUserIDBMaxDataLength() (3)

//! Create the ESC_UnlockSettings packet from parameters
void encodeESC_UnlockSettingsPacket(void* pkt);

//! Decode the ESC_UnlockSettings packet to parameters
int decodeESC_UnlockSettingsPacket(const void* pkt);

//! return the packet ID for the ESC_UnlockSettings packet
#define getESC_UnlockSettingsPacketID() (PKT_ESC_SYSTEM_CMD)

//! return the minimum encoded length for the ESC_UnlockSettings packet
#define getESC_UnlockSettingsMinDataLength() (4)

//! return the maximum encoded length for the ESC_UnlockSettings packet
#define getESC_UnlockSettingsMaxDataLength() (4)

//! Create the ESC_LockSettings packet from parameters
void encodeESC_LockSettingsPacket(void* pkt);

//! Decode the ESC_LockSettings packet to parameters
int decodeESC_LockSettingsPacket(const void* pkt);

//! return the packet ID for the ESC_LockSettings packet
#define getESC_LockSettingsPacketID() (PKT_ESC_SYSTEM_CMD)

//! return the minimum encoded length for the ESC_LockSettings packet
#define getESC_LockSettingsMinDataLength() (4)

//! return the maximum encoded length for the ESC_LockSettings packet
#define getESC_LockSettingsMaxDataLength() (4)

//! Create the ESC_ValidateSettings packet from parameters
void encodeESC_ValidateSettingsPacket(void* pkt);

//! Decode the ESC_ValidateSettings packet to parameters
int decodeESC_ValidateSettingsPacket(const void* pkt);

//! return the packet ID for the ESC_ValidateSettings packet
#define getESC_ValidateSettingsPacketID() (PKT_ESC_SYSTEM_CMD)

//! return the minimum encoded length for the ESC_ValidateSettings packet
#define getESC_ValidateSettingsMinDataLength() (4)

//! return the maximum encoded length for the ESC_ValidateSettings packet
#define getESC_ValidateSettingsMaxDataLength() (4)

//! Create the ESC_ResetMotorRunTime packet from parameters
void encodeESC_ResetMotorRunTimePacket(void* pkt, uint16_t serialNumber);

//! Decode the ESC_ResetMotorRunTime packet to parameters
int decodeESC_ResetMotorRunTimePacket(const void* pkt, uint16_t* serialNumber);

//! return the packet ID for the ESC_ResetMotorRunTime packet
#define getESC_ResetMotorRunTimePacketID() (PKT_ESC_SYSTEM_CMD)

//! return the minimum encoded length for the ESC_ResetMotorRunTime packet
#define getESC_ResetMotorRunTimeMinDataLength() (5)

//! return the maximum encoded length for the ESC_ResetMotorRunTime packet
#define getESC_ResetMotorRunTimeMaxDataLength() (5)

//! Create the ESC_EnterBootloader packet from parameters
void encodeESC_EnterBootloaderPacket(void* pkt);

//! Decode the ESC_EnterBootloader packet to parameters
int decodeESC_EnterBootloaderPacket(const void* pkt);

//! return the packet ID for the ESC_EnterBootloader packet
#define getESC_EnterBootloaderPacketID() (PKT_ESC_SYSTEM_CMD)

//! return the minimum encoded length for the ESC_EnterBootloader packet
#define getESC_EnterBootloaderMinDataLength() (3)

//! return the maximum encoded length for the ESC_EnterBootloader packet
#define getESC_EnterBootloaderMaxDataLength() (3)

//! Create the ESC_ResetESC packet from parameters
void encodeESC_ResetESCPacket(void* pkt);

//! Decode the ESC_ResetESC packet to parameters
int decodeESC_ResetESCPacket(const void* pkt);

//! return the packet ID for the ESC_ResetESC packet
#define getESC_ResetESCPacketID() (PKT_ESC_SYSTEM_CMD)

//! return the minimum encoded length for the ESC_ResetESC packet
#define getESC_ResetESCMinDataLength() (3)

//! return the maximum encoded length for the ESC_ResetESC packet
#define getESC_ResetESCMaxDataLength() (3)

#ifdef __cplusplus
}
#endif
#endif // _ESCCOMMANDS_H
