// ESCDefines.c was generated by ProtoGen version 3.2.a

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

#include "ESCDefines.h"
#include "fielddecode.h"
#include "fieldencode.h"
#include "scaleddecode.h"
#include "scaledencode.h"

/*!
 * \brief Encode a ESC_StatusBits_t into a byte array
 *
 * The *status* of the ESC is represented using these status bits. ESC system
 * functionality can be quickly determined using these bits
 * \param _pg_data points to the byte array to add encoded data to
 * \param _pg_bytecount points to the starting location in the byte array, and will be incremented by the number of encoded bytes.
 * \param _pg_user is the data to encode in the byte array
 */
void encodeESC_StatusBits_t(uint8_t* _pg_data, int* _pg_bytecount, const ESC_StatusBits_t* _pg_user)
{
    int _pg_byteindex = *_pg_bytecount;

    // Set if hardware inhibit is active (ESC is disabled)
    _pg_data[_pg_byteindex] = (uint8_t)((_pg_user->hwInhibit == true) ? 1 : 0) << 7;

    // Set if software inhibit is active (ESC is disabled)
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->swInhibit == true) ? 1 : 0) << 6;

    // Set if Active Freewheeling is currently active
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->afwEnabled == true) ? 1 : 0) << 5;

    // 0 = Motor direction is FORWARDS, 1= Motor direction is REVERSE
    _pg_data[_pg_byteindex] |= (uint8_t)limitMax(_pg_user->direction, 1) << 4;

    // Set if the ESC command timeout period has elapsed (and the ESC is in STANDBY mode)
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->timeout == true) ? 1 : 0) << 3;

    // Set if ESC is in starting mode (Cleared if ESC is stopped or running)
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->starting == true) ? 1 : 0) << 2;

    // 0 = most recent command from CAN, 1 = most recent command from PWM
    _pg_data[_pg_byteindex] |= (uint8_t)limitMax(_pg_user->commandSource, 1) << 1;

    // Set if ESC is running
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->running == true) ? 1 : 0);

    // Warning active - refer to the PKT_ESC_WARNINGS_ERRORS packet
    _pg_data[_pg_byteindex + 1] = (uint8_t)((_pg_user->anyWarnings == true) ? 1 : 0) << 7;

    // Error active - refer to the PKT_ESC_WARNINGS_ERRORS packet
    _pg_data[_pg_byteindex + 1] |= (uint8_t)((_pg_user->anyErrors == true) ? 1 : 0) << 6;

    // Reserved bits for future use

    // Set if the motor is spinning (even if it is not being driven)
    _pg_data[_pg_byteindex + 1] |= (uint8_t)((_pg_user->spinning == true) ? 1 : 0);

    // Set if motor is spinning opposite to configured rotation direction
    _pg_data[_pg_byteindex + 2] = (uint8_t)((_pg_user->spinningReversed == true) ? 1 : 0) << 7;

    // Set if motor duty cycle is being limited due to ESC protection settings
    _pg_data[_pg_byteindex + 2] |= (uint8_t)((_pg_user->foldback == true) ? 1 : 0) << 6;

    // Set if the ESC is attempting to sync with the motor
    _pg_data[_pg_byteindex + 2] |= (uint8_t)((_pg_user->syncing == true) ? 1 : 0) << 5;

    // Reserved bits for future use

    // Set if the ESC is in debug mode (factory use only)
    _pg_data[_pg_byteindex + 2] |= (uint8_t)((_pg_user->debug == true) ? 1 : 0);
    _pg_byteindex += 3; // close bit field

    *_pg_bytecount = _pg_byteindex;

}// encodeESC_StatusBits_t

/*!
 * \brief Decode a ESC_StatusBits_t from a byte array
 *
 * The *status* of the ESC is represented using these status bits. ESC system
 * functionality can be quickly determined using these bits
 * \param _pg_data points to the byte array to decoded data from
 * \param _pg_bytecount points to the starting location in the byte array, and will be incremented by the number of bytes decoded
 * \param _pg_user is the data to decode from the byte array
 * \return 1 if the data are decoded, else 0.
 */
int decodeESC_StatusBits_t(const uint8_t* _pg_data, int* _pg_bytecount, ESC_StatusBits_t* _pg_user)
{
    int _pg_byteindex = *_pg_bytecount;

    // Set if hardware inhibit is active (ESC is disabled)
    _pg_user->hwInhibit = ((_pg_data[_pg_byteindex] >> 7)) ? true : false;

    // Set if software inhibit is active (ESC is disabled)
    _pg_user->swInhibit = (((_pg_data[_pg_byteindex] >> 6) & 0x1)) ? true : false;

    // Set if Active Freewheeling is currently active
    _pg_user->afwEnabled = (((_pg_data[_pg_byteindex] >> 5) & 0x1)) ? true : false;

    // 0 = Motor direction is FORWARDS, 1= Motor direction is REVERSE
    _pg_user->direction = ((_pg_data[_pg_byteindex] >> 4) & 0x1);

    // Set if the ESC command timeout period has elapsed (and the ESC is in STANDBY mode)
    _pg_user->timeout = (((_pg_data[_pg_byteindex] >> 3) & 0x1)) ? true : false;

    // Set if ESC is in starting mode (Cleared if ESC is stopped or running)
    _pg_user->starting = (((_pg_data[_pg_byteindex] >> 2) & 0x1)) ? true : false;

    // 0 = most recent command from CAN, 1 = most recent command from PWM
    _pg_user->commandSource = ((_pg_data[_pg_byteindex] >> 1) & 0x1);

    // Set if ESC is running
    _pg_user->running = (((_pg_data[_pg_byteindex]) & 0x1)) ? true : false;

    // Warning active - refer to the PKT_ESC_WARNINGS_ERRORS packet
    _pg_user->anyWarnings = ((_pg_data[_pg_byteindex + 1] >> 7)) ? true : false;

    // Error active - refer to the PKT_ESC_WARNINGS_ERRORS packet
    _pg_user->anyErrors = (((_pg_data[_pg_byteindex + 1] >> 6) & 0x1)) ? true : false;

    // Reserved bits for future use

    // Set if the motor is spinning (even if it is not being driven)
    _pg_user->spinning = (((_pg_data[_pg_byteindex + 1]) & 0x1)) ? true : false;

    // Set if motor is spinning opposite to configured rotation direction
    _pg_user->spinningReversed = ((_pg_data[_pg_byteindex + 2] >> 7)) ? true : false;

    // Set if motor duty cycle is being limited due to ESC protection settings
    _pg_user->foldback = (((_pg_data[_pg_byteindex + 2] >> 6) & 0x1)) ? true : false;

    // Set if the ESC is attempting to sync with the motor
    _pg_user->syncing = (((_pg_data[_pg_byteindex + 2] >> 5) & 0x1)) ? true : false;

    // Reserved bits for future use

    // Set if the ESC is in debug mode (factory use only)
    _pg_user->debug = (((_pg_data[_pg_byteindex + 2]) & 0x1)) ? true : false;
    _pg_byteindex += 3; // close bit field

    *_pg_bytecount = _pg_byteindex;

    return 1;

}// decodeESC_StatusBits_t

/*!
 * \brief Encode a ESC_WarningBits_t into a byte array
 *
 * The *warning* bits enumerate various system warnings/errors of which the
 * user (or user software) should be made aware. These *warning* bits are
 * transmitted in the telemetry packets such that user software is aware of any
 * these *warning* conditions and can poll the ESC for particular packets if
 * any further information is needed. The ESC will continue to function in the
 * case of a *warning* state
 * \param _pg_data points to the byte array to add encoded data to
 * \param _pg_bytecount points to the starting location in the byte array, and will be incremented by the number of encoded bytes.
 * \param _pg_user is the data to encode in the byte array
 */
void encodeESC_WarningBits_t(uint8_t* _pg_data, int* _pg_bytecount, const ESC_WarningBits_t* _pg_user)
{
    int _pg_byteindex = *_pg_bytecount;

    // Reserved for future use
    _pg_data[_pg_byteindex] = 0;

    // Set if the ESC motor speed exceeds the configured warning threshold
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->overspeed == true) ? 1 : 0) << 6;

    // Set if the ESC motor current (positive or negative) exceeds the configured warning threshold
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->overcurrent == true) ? 1 : 0) << 5;

    // Set if the internal ESC temperature is above the warning threshold
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->escTemperature == true) ? 1 : 0) << 4;

    // Set if the motor temperature is above the warning threshold
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->motorTemperature == true) ? 1 : 0) << 3;

    // Set if the input voltage is below the minimum threshold
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->undervoltage == true) ? 1 : 0) << 2;

    // Set if the input voltage is above the maximum threshold
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->overvoltage == true) ? 1 : 0) << 1;

    // Set if hardware PWM input is enabled but invalid
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->invalidPWMsignal == true) ? 1 : 0);

    // Set if the motor demag angle exceeds the maximum threshold
    _pg_data[_pg_byteindex + 1] = (uint8_t)((_pg_user->demagAngle == true) ? 1 : 0) << 7;

    // Set if the auto-advance exceeds 25 degrees
    _pg_data[_pg_byteindex + 1] |= (uint8_t)((_pg_user->advanceLimit == true) ? 1 : 0) << 6;

    // Set if the measured demag pulse is exceptionally long
    _pg_data[_pg_byteindex + 1] |= (uint8_t)((_pg_user->longDemag == true) ? 1 : 0) << 5;

    // Set if a zero-crossing measurement was missed
    _pg_data[_pg_byteindex + 1] |= (uint8_t)((_pg_user->missedZeroCrossing == true) ? 1 : 0) << 4;

    // Motor is spinning in the wrong direction
    _pg_data[_pg_byteindex + 1] |= (uint8_t)((_pg_user->spinningReversed == true) ? 1 : 0) << 3;

    // Motor has reached maximum allowable commutation speed
    _pg_data[_pg_byteindex + 1] |= (uint8_t)((_pg_user->commSpeedLimit == true) ? 1 : 0) << 2;

    // Settings checksum does not match programmed value
    _pg_data[_pg_byteindex + 1] |= (uint8_t)((_pg_user->settingsChecksum == true) ? 1 : 0) << 1;

    // Reserved for future use
    _pg_byteindex += 2; // close bit field

    *_pg_bytecount = _pg_byteindex;

}// encodeESC_WarningBits_t

/*!
 * \brief Decode a ESC_WarningBits_t from a byte array
 *
 * The *warning* bits enumerate various system warnings/errors of which the
 * user (or user software) should be made aware. These *warning* bits are
 * transmitted in the telemetry packets such that user software is aware of any
 * these *warning* conditions and can poll the ESC for particular packets if
 * any further information is needed. The ESC will continue to function in the
 * case of a *warning* state
 * \param _pg_data points to the byte array to decoded data from
 * \param _pg_bytecount points to the starting location in the byte array, and will be incremented by the number of bytes decoded
 * \param _pg_user is the data to decode from the byte array
 * \return 1 if the data are decoded, else 0.
 */
int decodeESC_WarningBits_t(const uint8_t* _pg_data, int* _pg_bytecount, ESC_WarningBits_t* _pg_user)
{
    int _pg_byteindex = *_pg_bytecount;

    // Reserved for future use

    // Set if the ESC motor speed exceeds the configured warning threshold
    _pg_user->overspeed = (((_pg_data[_pg_byteindex] >> 6) & 0x1)) ? true : false;

    // Set if the ESC motor current (positive or negative) exceeds the configured warning threshold
    _pg_user->overcurrent = (((_pg_data[_pg_byteindex] >> 5) & 0x1)) ? true : false;

    // Set if the internal ESC temperature is above the warning threshold
    _pg_user->escTemperature = (((_pg_data[_pg_byteindex] >> 4) & 0x1)) ? true : false;

    // Set if the motor temperature is above the warning threshold
    _pg_user->motorTemperature = (((_pg_data[_pg_byteindex] >> 3) & 0x1)) ? true : false;

    // Set if the input voltage is below the minimum threshold
    _pg_user->undervoltage = (((_pg_data[_pg_byteindex] >> 2) & 0x1)) ? true : false;

    // Set if the input voltage is above the maximum threshold
    _pg_user->overvoltage = (((_pg_data[_pg_byteindex] >> 1) & 0x1)) ? true : false;

    // Set if hardware PWM input is enabled but invalid
    _pg_user->invalidPWMsignal = (((_pg_data[_pg_byteindex]) & 0x1)) ? true : false;

    // Set if the motor demag angle exceeds the maximum threshold
    _pg_user->demagAngle = ((_pg_data[_pg_byteindex + 1] >> 7)) ? true : false;

    // Set if the auto-advance exceeds 25 degrees
    _pg_user->advanceLimit = (((_pg_data[_pg_byteindex + 1] >> 6) & 0x1)) ? true : false;

    // Set if the measured demag pulse is exceptionally long
    _pg_user->longDemag = (((_pg_data[_pg_byteindex + 1] >> 5) & 0x1)) ? true : false;

    // Set if a zero-crossing measurement was missed
    _pg_user->missedZeroCrossing = (((_pg_data[_pg_byteindex + 1] >> 4) & 0x1)) ? true : false;

    // Motor is spinning in the wrong direction
    _pg_user->spinningReversed = (((_pg_data[_pg_byteindex + 1] >> 3) & 0x1)) ? true : false;

    // Motor has reached maximum allowable commutation speed
    _pg_user->commSpeedLimit = (((_pg_data[_pg_byteindex + 1] >> 2) & 0x1)) ? true : false;

    // Settings checksum does not match programmed value
    _pg_user->settingsChecksum = (((_pg_data[_pg_byteindex + 1] >> 1) & 0x1)) ? true : false;

    // Reserved for future use
    _pg_byteindex += 2; // close bit field

    *_pg_bytecount = _pg_byteindex;

    return 1;

}// decodeESC_WarningBits_t

/*!
 * \brief Encode a ESC_ErrorBits_t into a byte array
 *
 * The *error* bits enumerate critical system errors that will cause the ESC to
 * stop functioning until the error cases are alleviated
 * \param _pg_data points to the byte array to add encoded data to
 * \param _pg_bytecount points to the starting location in the byte array, and will be incremented by the number of encoded bytes.
 * \param _pg_user is the data to encode in the byte array
 */
void encodeESC_ErrorBits_t(uint8_t* _pg_data, int* _pg_bytecount, const ESC_ErrorBits_t* _pg_user)
{
    int _pg_byteindex = *_pg_bytecount;

    // Set if the ESC failed to start the motor
    _pg_data[_pg_byteindex] = (uint8_t)((_pg_user->failedStart == true) ? 1 : 0) << 7;

    // Lost commutation
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->commutation == true) ? 1 : 0) << 6;

    // Set if hall sensor error detected
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->hallSensor == true) ? 1 : 0) << 5;

    // Current exceeded hard-limit
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->overcurrent == true) ? 1 : 0) << 4;

    // Temperature exceeded hard-limit
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->overtemperature == true) ? 1 : 0) << 3;

    // Motor commutation speed exceeded hard-limit
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->overspeed == true) ? 1 : 0) << 2;

    // Motor stopped due to high demag angle
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->demag == true) ? 1 : 0) << 1;

    // Reserved for future use
    _pg_data[_pg_byteindex + 1] = 0;

    // Reserved for future use
    _pg_byteindex += 2; // close bit field

    *_pg_bytecount = _pg_byteindex;

}// encodeESC_ErrorBits_t

/*!
 * \brief Decode a ESC_ErrorBits_t from a byte array
 *
 * The *error* bits enumerate critical system errors that will cause the ESC to
 * stop functioning until the error cases are alleviated
 * \param _pg_data points to the byte array to decoded data from
 * \param _pg_bytecount points to the starting location in the byte array, and will be incremented by the number of bytes decoded
 * \param _pg_user is the data to decode from the byte array
 * \return 1 if the data are decoded, else 0.
 */
int decodeESC_ErrorBits_t(const uint8_t* _pg_data, int* _pg_bytecount, ESC_ErrorBits_t* _pg_user)
{
    int _pg_byteindex = *_pg_bytecount;

    // Set if the ESC failed to start the motor
    _pg_user->failedStart = ((_pg_data[_pg_byteindex] >> 7)) ? true : false;

    // Lost commutation
    _pg_user->commutation = (((_pg_data[_pg_byteindex] >> 6) & 0x1)) ? true : false;

    // Set if hall sensor error detected
    _pg_user->hallSensor = (((_pg_data[_pg_byteindex] >> 5) & 0x1)) ? true : false;

    // Current exceeded hard-limit
    _pg_user->overcurrent = (((_pg_data[_pg_byteindex] >> 4) & 0x1)) ? true : false;

    // Temperature exceeded hard-limit
    _pg_user->overtemperature = (((_pg_data[_pg_byteindex] >> 3) & 0x1)) ? true : false;

    // Motor commutation speed exceeded hard-limit
    _pg_user->overspeed = (((_pg_data[_pg_byteindex] >> 2) & 0x1)) ? true : false;

    // Motor stopped due to high demag angle
    _pg_user->demag = (((_pg_data[_pg_byteindex] >> 1) & 0x1)) ? true : false;

    // Reserved for future use

    // Reserved for future use
    _pg_byteindex += 2; // close bit field

    *_pg_bytecount = _pg_byteindex;

    return 1;

}// decodeESC_ErrorBits_t

/*!
 * \brief Encode a ESC_TelemetryPackets_t into a byte array
 *
 * These bits are used to determine which packets are automatically transmitted
 * as telemetry data by the ESC. Only the packets described here can be
 * configured as telemetry packets
 * \param _pg_data points to the byte array to add encoded data to
 * \param _pg_bytecount points to the starting location in the byte array, and will be incremented by the number of encoded bytes.
 * \param _pg_user is the data to encode in the byte array
 */
void encodeESC_TelemetryPackets_t(uint8_t* _pg_data, int* _pg_bytecount, const ESC_TelemetryPackets_t* _pg_user)
{
    int _pg_byteindex = *_pg_bytecount;

    // If this bit is set, the STATUS_A packet will be transmitted at the configured rate
    _pg_data[_pg_byteindex] = (uint8_t)((_pg_user->statusA == true) ? 1 : 0) << 7;

    // If this bit is set, the STATUS_B packet will be transmitted at the configured rate
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->statusB == true) ? 1 : 0) << 6;

    // If this bit is set, the STATUS_C packet will be transmitted at the configured rate
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->statusC == true) ? 1 : 0) << 5;

    // If this bit is set, the ACCELEROMETER packet will be transmitted at the configured rate
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->accelerometer == true) ? 1 : 0) << 4;

    // If this bit is set, the STATUS_D packet will be transmitted at the configured rate
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->statusD == true) ? 1 : 0) << 3;

    // Reserved for future use
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->reservedTelemA == true) ? 1 : 0) << 2;

    // If this bit is set, any STATUS_x packets selected for telemetry will be mirrored on the Piccolo Downlink packet group (0x14)
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->piccoloDownlink == true) ? 1 : 0) << 1;

    // Reserved for future use
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->reservedTelemC == true) ? 1 : 0);
    _pg_byteindex += 1; // close bit field

    *_pg_bytecount = _pg_byteindex;

}// encodeESC_TelemetryPackets_t

/*!
 * \brief Decode a ESC_TelemetryPackets_t from a byte array
 *
 * These bits are used to determine which packets are automatically transmitted
 * as telemetry data by the ESC. Only the packets described here can be
 * configured as telemetry packets
 * \param _pg_data points to the byte array to decoded data from
 * \param _pg_bytecount points to the starting location in the byte array, and will be incremented by the number of bytes decoded
 * \param _pg_user is the data to decode from the byte array
 * \return 1 if the data are decoded, else 0.
 */
int decodeESC_TelemetryPackets_t(const uint8_t* _pg_data, int* _pg_bytecount, ESC_TelemetryPackets_t* _pg_user)
{
    int _pg_byteindex = *_pg_bytecount;

    // If this bit is set, the STATUS_A packet will be transmitted at the configured rate
    _pg_user->statusA = ((_pg_data[_pg_byteindex] >> 7)) ? true : false;

    // If this bit is set, the STATUS_B packet will be transmitted at the configured rate
    _pg_user->statusB = (((_pg_data[_pg_byteindex] >> 6) & 0x1)) ? true : false;

    // If this bit is set, the STATUS_C packet will be transmitted at the configured rate
    _pg_user->statusC = (((_pg_data[_pg_byteindex] >> 5) & 0x1)) ? true : false;

    // If this bit is set, the ACCELEROMETER packet will be transmitted at the configured rate
    _pg_user->accelerometer = (((_pg_data[_pg_byteindex] >> 4) & 0x1)) ? true : false;

    // If this bit is set, the STATUS_D packet will be transmitted at the configured rate
    _pg_user->statusD = (((_pg_data[_pg_byteindex] >> 3) & 0x1)) ? true : false;

    // Reserved for future use
    _pg_user->reservedTelemA = (((_pg_data[_pg_byteindex] >> 2) & 0x1)) ? true : false;

    // If this bit is set, any STATUS_x packets selected for telemetry will be mirrored on the Piccolo Downlink packet group (0x14)
    _pg_user->piccoloDownlink = (((_pg_data[_pg_byteindex] >> 1) & 0x1)) ? true : false;

    // Reserved for future use
    _pg_user->reservedTelemC = (((_pg_data[_pg_byteindex]) & 0x1)) ? true : false;
    _pg_byteindex += 1; // close bit field

    *_pg_bytecount = _pg_byteindex;

    return 1;

}// decodeESC_TelemetryPackets_t

/*!
 * \brief Encode a ESC_DebugPackets_t into a byte array
 *
 * These bits are used to select which debug packets are transmitted at
 * high-frequency by the ESC
 * \param _pg_data points to the byte array to add encoded data to
 * \param _pg_bytecount points to the starting location in the byte array, and will be incremented by the number of encoded bytes.
 * \param _pg_user is the data to encode in the byte array
 */
void encodeESC_DebugPackets_t(uint8_t* _pg_data, int* _pg_bytecount, const ESC_DebugPackets_t* _pg_user)
{
    int _pg_byteindex = *_pg_bytecount;

    // Control loop terms
    _pg_data[_pg_byteindex] = (uint8_t)((_pg_user->ctrlLoopOutputs == true) ? 1 : 0) << 7;

    // Hall sensor debug information
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->hallSensors == true) ? 1 : 0) << 6;

    // Commutation debug information
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->commutation == true) ? 1 : 0) << 5;

    // Demag debug information
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->demag == true) ? 1 : 0) << 4;

    // PWM input debug information
    _pg_data[_pg_byteindex] |= (uint8_t)((_pg_user->pwmInput == true) ? 1 : 0) << 3;

    // Reserved for future use
    _pg_byteindex += 1; // close bit field

    // Reserved for future use
    uint8ToBytes((uint8_t)(0), _pg_data, &_pg_byteindex);

    *_pg_bytecount = _pg_byteindex;

}// encodeESC_DebugPackets_t

/*!
 * \brief Decode a ESC_DebugPackets_t from a byte array
 *
 * These bits are used to select which debug packets are transmitted at
 * high-frequency by the ESC
 * \param _pg_data points to the byte array to decoded data from
 * \param _pg_bytecount points to the starting location in the byte array, and will be incremented by the number of bytes decoded
 * \param _pg_user is the data to decode from the byte array
 * \return 1 if the data are decoded, else 0.
 */
int decodeESC_DebugPackets_t(const uint8_t* _pg_data, int* _pg_bytecount, ESC_DebugPackets_t* _pg_user)
{
    int _pg_byteindex = *_pg_bytecount;

    // Control loop terms
    _pg_user->ctrlLoopOutputs = ((_pg_data[_pg_byteindex] >> 7)) ? true : false;

    // Hall sensor debug information
    _pg_user->hallSensors = (((_pg_data[_pg_byteindex] >> 6) & 0x1)) ? true : false;

    // Commutation debug information
    _pg_user->commutation = (((_pg_data[_pg_byteindex] >> 5) & 0x1)) ? true : false;

    // Demag debug information
    _pg_user->demag = (((_pg_data[_pg_byteindex] >> 4) & 0x1)) ? true : false;

    // PWM input debug information
    _pg_user->pwmInput = (((_pg_data[_pg_byteindex] >> 3) & 0x1)) ? true : false;

    // Reserved for future use
    _pg_byteindex += 1; // close bit field

    // Reserved for future use
    _pg_byteindex += 1;

    *_pg_bytecount = _pg_byteindex;

    return 1;

}// decodeESC_DebugPackets_t

// end of ESCDefines.c
