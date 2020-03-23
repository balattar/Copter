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
/*
  SRXL2 protocol decoder using Horizon Hobby's open source library https://github.com/SpektrumRC/SRXL2
  Code by Andy Piper
 */

#include "spm_srxl.h"

#include "AP_RCProtocol.h"
#include "AP_RCProtocol_SRXL.h"
#include "AP_RCProtocol_SRXL2.h"
#include <AP_Math/AP_Math.h>
#include <AP_Spektrum_Telem/AP_Spektrum_Telem.h>
#include <AP_Vehicle/AP_Vehicle_Type.h>

extern const AP_HAL::HAL& hal;
//#define SRXL2_DEBUG
#ifdef SRXL2_DEBUG
# define debug(fmt, args...)	hal.console->printf("SRXL2:" fmt "\n", ##args)
#else
# define debug(fmt, args...)	do {} while(0)
#endif

AP_RCProtocol_SRXL2* AP_RCProtocol_SRXL2::_singleton;

AP_RCProtocol_SRXL2::AP_RCProtocol_SRXL2(AP_RCProtocol &_frontend) : AP_RCProtocol_Backend(_frontend)
{
    const uint32_t uniqueID = AP_HAL::micros();

    if (_singleton != nullptr) {
        AP_HAL::panic("Duplicate SRXL2 handler");
    }

    _singleton = this;
    // Init the local SRXL device
    if (!srxlInitDevice(SRXL_DEVICE_ID, SRXL_DEVICE_PRIORITY, SRXL_DEVICE_INFO, uniqueID)) {
        AP_HAL::panic("Failed to initialize SRXL2 device");
    }

    // Init the SRXL bus: The bus index must always be < SRXL_NUM_OF_BUSES -- in this case, it can only be 0
    if (!srxlInitBus(0, this, SRXL_SUPPORTED_BAUD_RATES)) {
        AP_HAL::panic("Failed to initialize SRXL2 bus");
    }

}

void AP_RCProtocol_SRXL2::process_pulse(uint32_t width_s0, uint32_t width_s1)
{
    uint8_t b;
    if (ss.process_pulse(width_s0, width_s1, b)) {
        _process_byte(ss.get_byte_timestamp_us(), b);
    }
}

void AP_RCProtocol_SRXL2::_process_byte(uint32_t timestamp_us, uint8_t byte)
{
    if (_decode_state == STATE_IDLE) {
        switch (byte) {
        case SPEKTRUM_SRXL_ID:
            _frame_len_full = 0U;
            _decode_state = STATE_NEW;
            break;
        default:
            _frame_len_full = 0U;
            _decode_state = STATE_IDLE;
            _buflen = 0;
            return;
        }
    }

    switch (_decode_state) {
    case STATE_NEW:  // buffer header byte and prepare for frame reception and decoding
        _buffer[0U]=byte;
        _buflen = 1U;
        _decode_state_next = STATE_COLLECT;
        break;

    case STATE_COLLECT: // receive all bytes. After reception decode frame and provide rc channel information to FMU
        _buffer[_buflen] = byte;
        _buflen++;

        // need a header to get the length
        if (_buflen < SRXL2_HEADER_LEN) {
            return;
        }

        // parse the length
        if (_buflen == SRXL2_HEADER_LEN) {
            _frame_len_full = _buffer[2];
            return;
        }

        if (_buflen > _frame_len_full) {
            // a logic bug in the state machine, this shouldn't happen
            _decode_state = STATE_IDLE;
            _buflen = 0;
            _frame_len_full = 0;
            return;
        }

        if (_buflen == _frame_len_full) {
            log_data(AP_RCProtocol::SRXL2, timestamp_us, _buffer, _buflen);
            // Try to parse SRXL packet -- this internally calls srxlRun() after packet is parsed and resets timeout
            if (srxlParsePacket(0, _buffer, _frame_len_full)) {
                add_input(MAX_CHANNELS, _channels, _in_failsafe, _new_rssi);
            }
            _last_run_ms = AP_HAL::millis();
            _decode_state_next = STATE_IDLE;
        } else {
            _decode_state_next = STATE_COLLECT;
        }
        break;

    default:
        break;
    }

    _decode_state = _decode_state_next;
    _last_data_us = timestamp_us;
}

void AP_RCProtocol_SRXL2::update(void)
{
#if 0 // it's not clear this is actually required
    if (frontend.protocol_detected() == AP_RCProtocol::SRXL2) {
        uint32_t now = AP_HAL::millis();
        // there have been no updates since we were last called
        const uint32_t delay = now - _last_run_ms;
        if (delay > 5) {
            srxlRun(0, delay);
            _last_run_ms = now;
        }
    }
#endif
}

void AP_RCProtocol_SRXL2::capture_scaled_input(const uint16_t *values, bool in_failsafe, int16_t new_rssi)
{
    AP_RCProtocol_SRXL2* srxl2 = AP_RCProtocol_SRXL2::get_singleton();

    if (srxl2 != nullptr) {
        srxl2->_capture_scaled_input(values, in_failsafe, new_rssi);
    }
}

// capture SRXL2 encoded values
void AP_RCProtocol_SRXL2::_capture_scaled_input(const uint16_t *values, bool in_failsafe, int16_t new_rssi)
{
    _in_failsafe = in_failsafe;
    // AP rssi: -1 for unknown, 0 for no link, 255 for maximum link
    // SRXL2 rssi: -ve rssi in dBM, +ve rssi in percentage
    if (new_rssi >= 0) {
        _new_rssi = new_rssi * 255 / 100;
    } else {
        // pretty much a guess
        _new_rssi = 255 - 255 * (-20 - new_rssi) / (-20 - 85);
    }

    for (uint8_t i = 0; i < MAX_CHANNELS; i++) {
        /*
         * Each channel data value is sent as an unsigned 16-bit value from 0 to 65532 (0xFFFC)
         * with 32768 (0x8000) representing "Servo Center". The channel value must be bit-shifted
         * to the right to match the applications's accepted resolution.
         *
         * So here we scale to DSMX-2048 and then use our regular Spektrum conversion.
         */
        _channels[i] = ((((int)(values[i] >> 5) - 1024) * 1000) / 1700) + 1500;
    }
}


// start bind on DSM satellites
void AP_RCProtocol_SRXL2::start_bind(void)
{
    srxlEnterBind(DSMX_11MS, true);
}

// process a byte provided by a uart
void AP_RCProtocol_SRXL2::process_byte(uint8_t byte, uint32_t baudrate)
{
    if (baudrate != 115200) {
        return;
    }
    _process_byte(AP_HAL::micros(), byte);
}

// send data to the uart
void AP_RCProtocol_SRXL2::send_on_uart(uint8_t* pBuffer, uint8_t length)
{
    AP_RCProtocol_SRXL2* srxl2 = AP_RCProtocol_SRXL2::get_singleton();

    if (srxl2 != nullptr) {
        srxl2->_send_on_uart(pBuffer, length);
    }
}

// send data to the uart
void AP_RCProtocol_SRXL2::_send_on_uart(uint8_t* pBuffer, uint8_t length)
{
    // writing at startup locks-up the flight controller
    if (have_UART() && AP_HAL::millis() > 3000) {
        // check that we haven't been too slow in responding to the new
        // UART data. If we respond too late then we will corrupt the next
        // incoming control frame
        uint64_t tend = get_UART()->receive_time_constraint_us(1);
        uint64_t now = AP_HAL::micros64();
        uint64_t tdelay = now - tend;
        if (tdelay > 2500) {
            // we've been too slow in responding
            return;
        }
        // debug telemetry packets
        if (pBuffer[1] == 0x80 && pBuffer[4] != 0) {
            debug("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x: %s",
                pBuffer[0], pBuffer[1], pBuffer[2], pBuffer[3], pBuffer[4], pBuffer[5], pBuffer[6], pBuffer[7], pBuffer[8], pBuffer[9], &pBuffer[7]);
        }
        get_UART()->write(pBuffer, length);
    }
}

// change the uart baud rate
void AP_RCProtocol_SRXL2::change_baud_rate(uint32_t baudrate)
{
    AP_RCProtocol_SRXL2* srxl2 = AP_RCProtocol_SRXL2::get_singleton();

    if (srxl2 != nullptr) {
        srxl2->_change_baud_rate(baudrate);
    }
}

// change the uart baud rate
void AP_RCProtocol_SRXL2::_change_baud_rate(uint32_t baudrate)
{
#if 0
    if (have_UART()) {
        get_UART()->begin(baudrate);
    }
#endif
}

// User-provided routine to change the baud rate settings on the given UART:
// uart - the same uint8_t value as the uart parameter passed to srxlInit()
// baudRate - the actual baud rate (currently either 115200 or 400000)
void srxlChangeBaudRate(void* this_ptr, uint32_t baudRate)
{
    AP_RCProtocol_SRXL2::change_baud_rate(baudRate);

}

// User-provided routine to actually transmit a packet on the given UART:
// uart - the same uint8_t value as the uart parameter passed to srxlInit()
// pBuffer - a pointer to an array of uint8_t values to send over the UART
// length - the number of bytes contained in pBuffer that should be sent
void srxlSendOnUart(void* this_ptr, uint8_t* pBuffer, uint8_t length)
{
    AP_RCProtocol_SRXL2::send_on_uart(pBuffer, length);
}

// User-provided callback routine to fill in the telemetry data to send to the master when requested:
// pTelemetryData - a pointer to the 16-byte SrxlTelemetryData transmit buffer to populate
// NOTE: srxlTelemData is available as a global variable, so the memcpy line commented out below
// could be used if you would prefer to just populate that with the next outgoing telemetry packet.
void srxlFillTelemetry(SrxlTelemetryData* pTelemetryData)
{
#if !APM_BUILD_TYPE(APM_BUILD_iofirmware)
    AP_Spektrum_Telem::get_telem_data(pTelemetryData->raw);
#endif
}

// User-provided callback routine that is called whenever a control data packet is received:
// pChannelData - a pointer to the received SrxlChannelData structure for manual parsing
// isFailsafe - true if channel data is set to failsafe values, else false.
// this is called from within srxlParsePacket() and before the SRXL2 state machine has been run
// so be very careful to only do local operations
void srxlReceivedChannelData(SrxlChannelData* pChannelData, bool isFailsafe)
{
    if (isFailsafe) {
        AP_RCProtocol_SRXL2::capture_scaled_input(pChannelData->values, true, pChannelData->rssi);
    } else {
        AP_RCProtocol_SRXL2::capture_scaled_input(srxlChData.values, false, srxlChData.rssi);
    }
}

// User-provided callback routine to handle reception of a bound data report (either requested or unprompted).
// Return true if you want this bind information set automatically for all other receivers on all SRXL buses.
bool srxlOnBind(SrxlFullID device, SrxlBindData info)
{
    // TODO: Add custom handling of bound data report here if needed
    return true;
}

// User-provided callback routine to handle reception of a VTX control packet.
void srxlOnVtx(SrxlVtxData* pVtxData)
{
    //userProvidedHandleVtxData(pVtxData);
}
