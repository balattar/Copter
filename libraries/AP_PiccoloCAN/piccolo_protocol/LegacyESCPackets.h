// LegacyESCPackets.h was generated by ProtoGen version 3.2.a

/*
 * This file is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Oliver Walters
 */

#ifndef _LEGACYESCPACKETS_H
#define _LEGACYESCPACKETS_H

// Language target is C, C++ compilers: don't mangle us
#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \file
 */

#include <stdbool.h>
#include "ESCVelocityProtocol.h"
#include "LegacyESCDefines.h"

/*!
 * Legacy (gen-1) definition for the STATUS_A packet
 */
typedef struct
{
    uint8_t                 mode;     //!< ESC operating mode. The lower four bits indicate the operational mode of the ESC, in accordance with the ESCOperatingModes enumeration. The upper 3 bits are used for debugging and should be ignored for general use.
    ESC_LegacyStatusBits_t  status;   //!< ESC status bits
    ESC_LegacyWarningBits_t warnings; //!< ESC warning bits
    ESC_LegacyErrorBits_t   errors;   //!< ESC *error* bits
    uint16_t                command;  //!< ESC operational command - value depends on 'mode' available in this packet. If the ESC is disabled, data reads 0x0000. If the ESC is in open-loop PWM mode, this value is the PWM command in units of 1us, in the range 1000us to 2000us. If the ESC is in closed-loop RPM mode, this value is the RPM command in units of 1RPM
    uint16_t                rpm;      //!< Motor speed
}ESC_LegacyStatusA_t;

//! Create the ESC_LegacyStatusA packet from parameters
void encodeESC_LegacyStatusAPacket(void* pkt, uint8_t mode, const ESC_LegacyStatusBits_t* status, const ESC_LegacyWarningBits_t* warnings, const ESC_LegacyErrorBits_t* errors, uint16_t command, uint16_t rpm);

//! Decode the ESC_LegacyStatusA packet to parameters
int decodeESC_LegacyStatusAPacket(const void* pkt, uint8_t* mode, ESC_LegacyStatusBits_t* status, ESC_LegacyWarningBits_t* warnings, ESC_LegacyErrorBits_t* errors, uint16_t* command, uint16_t* rpm);

//! return the packet ID for the ESC_LegacyStatusA packet
#define getESC_LegacyStatusAPacketID() (PKT_ESC_STATUS_A)

//! return the minimum encoded length for the ESC_LegacyStatusA packet
#define getESC_LegacyStatusAMinDataLength() (8)

//! return the maximum encoded length for the ESC_LegacyStatusA packet
#define getESC_LegacyStatusAMaxDataLength() (8)

#ifdef __cplusplus
}
#endif
#endif // _LEGACYESCPACKETS_H
