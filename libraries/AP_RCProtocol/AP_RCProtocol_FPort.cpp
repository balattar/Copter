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
 */
/*
  FRSky FPort implementation, with thanks to BetaFlight for
  specification and code reference
 */

#include "AP_RCProtocol_FPort.h"
#include <AP_Vehicle/AP_Vehicle_Type.h>
#include <AP_Frsky_Telem/AP_Frsky_Telem.h>
#include <AP_Vehicle/AP_Vehicle_Type.h>
#include <RC_Channel/RC_Channel.h>

extern const AP_HAL::HAL& hal;

#define FRAME_HEAD 0x7E
#define FRAME_DLE  0x7D
#define FRAME_XOR  0x20
#define FRAME_LEN_CONTROL 0x19
#define FRAME_LEN_DOWNLINK 0x08
#define MIN_FRAME_SIZE 12
#define MAX_CHANNELS 16

#define FLAGS_FAILSAFE_BIT	3
#define FLAGS_FRAMELOST_BIT	2

#define CHAN_SCALE_FACTOR ((2000.0 - 1000.0) / (1800.0 - 200.0))
#define CHAN_SCALE_OFFSET (int)(1000.0 - (CHAN_SCALE_FACTOR * 200.0 + 0.5f))

#define FPORT_TYPE_CONTROL 0
#define FPORT_TYPE_DOWNLINK 1

struct PACKED FPort_Frame {
    uint8_t header; // 0x7E
    uint8_t len;    // 0x19 for control, 0x08 for downlink
    uint8_t type;
    union {
        struct PACKED {
            uint16_t chan0 : 11;
            uint16_t chan1 : 11;
            uint16_t chan2 : 11;
            uint16_t chan3 : 11;
            uint16_t chan4 : 11;
            uint16_t chan5 : 11;
            uint16_t chan6 : 11;
            uint16_t chan7 : 11;
            uint16_t chan8 : 11;
            uint16_t chan9 : 11;
            uint16_t chan10 : 11;
            uint16_t chan11 : 11;
            uint16_t chan12 : 11;
            uint16_t chan13 : 11;
            uint16_t chan14 : 11;
            uint16_t chan15 : 11;
            uint8_t flags;
            uint8_t rssi;
            uint8_t crc;
            uint8_t end;
        } control;
        struct {
            uint8_t prim;
            uint16_t appid;
            uint8_t data[4];
            uint8_t crc;
            uint8_t end;
        } downlink;
    };
};

static_assert(sizeof(FPort_Frame) == FPORT_CONTROL_FRAME_SIZE, "FPort_Frame incorrect size");

// constructor
AP_RCProtocol_FPort::AP_RCProtocol_FPort(AP_RCProtocol &_frontend, bool _inverted) :
    AP_RCProtocol_Backend(_frontend),
    inverted(_inverted)
{}

// decode a full FPort control frame
void AP_RCProtocol_FPort::decode_control(const FPort_Frame &frame)
{
    uint16_t values[MAX_CHANNELS];

    // pull out of bitfields
    values[0] = frame.control.chan0;
    values[1] = frame.control.chan1;
    values[2] = frame.control.chan2;
    values[3] = frame.control.chan3;
    values[4] = frame.control.chan4;
    values[5] = frame.control.chan5;
    values[6] = frame.control.chan6;
    values[7] = frame.control.chan7;
    values[8] = frame.control.chan8;
    values[9] = frame.control.chan9;
    values[10] = frame.control.chan10;
    values[11] = frame.control.chan11;
    values[12] = frame.control.chan12;
    values[13] = frame.control.chan13;
    values[14] = frame.control.chan14;
    values[15] = frame.control.chan15;

    // scale values
    for (uint8_t i=0; i<MAX_CHANNELS; i++) {
        values[i] = (uint16_t)(values[i] * CHAN_SCALE_FACTOR + 0.5f) + CHAN_SCALE_OFFSET;
    }

    bool failsafe = ((frame.control.flags & (1 << FLAGS_FAILSAFE_BIT)) != 0);
    add_input(MAX_CHANNELS, values, failsafe, frame.control.rssi);
}

/*
  decode a full FPort downlink frame
*/
void AP_RCProtocol_FPort::decode_downlink(const FPort_Frame &frame)
{
#if !APM_BUILD_TYPE(APM_BUILD_iofirmware)
    if (frame.downlink.prim != 0x10 && frame.downlink.prim != 0x00) {
        // only respond to frame types 0x00 or 0x10
        return;
    }
    /*
      if we are getting FPORT over a UART then we can ask the FrSky
      telem library for some passthrough data to send back, enabling
      telemetry on the receiver via the same uart pin as we use for
      incoming RC frames
     */

    AP_HAL::UARTDriver *uart = get_UART();
    if (!uart) {
        return;
    }

    /*
      check that we haven't been too slow in responding to the new
      UART data. If we respond too late then we will corrupt the next
      incoming control frame
     */
    uint64_t tend = uart->receive_time_constraint_us(1);
    uint64_t now = AP_HAL::micros64();
    uint64_t tdelay = now - tend;
    if (tdelay > 2500) {
        // we've been too slow in responding
        return;
    }

    /*
      get the SPort data from FRSky_Telem and send it as an uplink
      packet
     */
    uint8_t frametype;
    uint16_t appid;
    uint32_t data;
    if (AP_Frsky_Telem::get_telem_data(frametype, appid, data)) {
        uint8_t buf[10];

        buf[0] = 0x08;
        buf[1] = 0x81;
        buf[2] = frametype;
        buf[3] = appid & 0xFF;
        buf[4] = appid >> 8;
        memcpy(&buf[5], &data, 4);

        uint16_t sum = 0;
        for (uint8_t i=0; i<sizeof(buf)-1; i++) {
            sum += buf[i];
        }
        sum = 0xff - ((sum & 0xff) + (sum >> 8));
        buf[9] = sum;

        // perform byte stuffing per FPort spec
        uint8_t len = 0;
        uint8_t buf2[sizeof(buf)*2+1];

        if (rc().fport_pad()) {
            // this padding helps on some uarts that have hw pullups
            buf2[len++] = 0xff;
        }

        for (uint8_t i=0; i<sizeof(buf); i++) {
            uint8_t c = buf[i];
            if (c == FRAME_DLE || buf[i] == FRAME_HEAD) {
                buf2[len++] = FRAME_DLE;
                buf2[len++] = c ^ FRAME_XOR;
            } else {
                buf2[len++] = c;
            }
        }

        uart->write(buf2, len);
    }
#endif
}

/*
  process a FPort input pulse of the given width
 */
void AP_RCProtocol_FPort::process_pulse(uint32_t width_s0, uint32_t width_s1)
{
    if (have_UART()) {
        // if we can use a UART we would much prefer to, as it allows
        // us to send SPORT data out
        return;
    }
    uint32_t w0 = width_s0;
    uint32_t w1 = width_s1;
    if (inverted) {
        w0 = saved_width;
        w1 = width_s0;
        saved_width = width_s1;
    }
    uint8_t b;
    if (ss.process_pulse(w0, w1, b)) {
        _process_byte(ss.get_byte_timestamp_us(), b);
    }
}

// support byte input
void AP_RCProtocol_FPort::_process_byte(uint32_t timestamp_us, uint8_t b)
{
    const bool have_frame_gap = (timestamp_us - byte_input.last_byte_us >= 2000U);

    byte_input.last_byte_us = timestamp_us;

    if (have_frame_gap) {
        // if we have a frame gap then this must be the start of a new
        // frame
        byte_input.ofs = 0;
        byte_input.got_DLE = false;
    }
    if (b != FRAME_HEAD && byte_input.ofs == 0) {
        // definately not FPort, missing header byte
        return;
    }

    // handle byte-stuffing decode
    if (byte_input.got_DLE) {
        b ^= FRAME_XOR;
        byte_input.got_DLE = false;
    } else if (b == FRAME_DLE) {
        byte_input.got_DLE = true;
        return;
    }

    byte_input.buf[byte_input.ofs++] = b;

    const FPort_Frame *frame = (const FPort_Frame *)&byte_input.buf[0];

    if (byte_input.ofs == 2) {
        // check for valid lengths
        if (frame->len != FRAME_LEN_CONTROL &&
            frame->len != FRAME_LEN_DOWNLINK) {
            // invalid, reset
            goto reset;
        }
    }

    if (byte_input.ofs == 3) {
        // check for valid lengths
        if ((frame->type == FPORT_TYPE_CONTROL && frame->len != FRAME_LEN_CONTROL) ||
            (frame->type == FPORT_TYPE_DOWNLINK && frame->len != FRAME_LEN_DOWNLINK)) {
            goto reset;
        }
    }

    if (frame->type == FPORT_TYPE_CONTROL && byte_input.ofs == FRAME_LEN_CONTROL + 4) {
        if (check_checksum()) {
            decode_control(*frame);
        }
        goto reset;
    } else if (frame->type == FPORT_TYPE_DOWNLINK && byte_input.ofs == FRAME_LEN_DOWNLINK + 4) {
        if (check_checksum()) {
            decode_downlink(*frame);
        }
        goto reset;
    }
    return;

reset:
    byte_input.ofs = 0;
    byte_input.got_DLE = false;
}

// check checksum byte
bool AP_RCProtocol_FPort::check_checksum(void)
{
    uint8_t len = byte_input.buf[1]+2;
    const uint8_t *b = &byte_input.buf[1];
    uint16_t sum = 0;
    for (uint8_t i=0; i<len; i++) {
        sum += b[i];
    }
    sum = (sum & 0xff) + (sum >> 8);
    return sum == 0xff;
}

// support byte input
void AP_RCProtocol_FPort::process_byte(uint8_t b, uint32_t baudrate)
{
    if (baudrate != 115200) {
        return;
    }
    _process_byte(AP_HAL::micros(), b);
}
