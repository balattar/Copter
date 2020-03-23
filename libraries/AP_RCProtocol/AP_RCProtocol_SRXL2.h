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

#pragma once

#include "AP_RCProtocol.h"
#include <AP_Math/AP_Math.h>
#include "SoftSerial.h"

#define SRXL2_MAX_CHANNELS 32U           /* Maximum number of channels from srxl2 datastream  */
#define SRXL2_FRAMELEN_MAX   80U      /* maximum possible framelengh  */
#define SRXL2_HEADER_LEN 3U

class AP_RCProtocol_SRXL2 : public AP_RCProtocol_Backend {
public:
    AP_RCProtocol_SRXL2(AP_RCProtocol &_frontend);
    void process_pulse(uint32_t width_s0, uint32_t width_s1) override;
    void process_byte(uint8_t byte, uint32_t baudrate) override;
    void start_bind(void) override;
    void update(void) override;
    // get singleton instance
    static AP_RCProtocol_SRXL2* get_singleton() {
        return _singleton;
    }

    // static functions for SRXL2 callbacks
    static void capture_scaled_input(const uint16_t *values, bool in_failsafe, int16_t rssi);
    static void send_on_uart(uint8_t* pBuffer, uint8_t length);
    static void change_baud_rate(uint32_t baudrate);

private:

    const uint8_t MAX_CHANNELS = MIN((uint8_t)SRXL_MAX_CHANNELS, (uint8_t)MAX_RCIN_CHANNELS);

    static AP_RCProtocol_SRXL2* _singleton;

    void _process_byte(uint32_t timestamp_us, uint8_t byte);
    void _send_on_uart(uint8_t* pBuffer, uint8_t length);
    void _change_baud_rate(uint32_t baudrate);
    void _capture_scaled_input(const uint16_t *values, bool in_failsafe, int16_t rssi);

    uint8_t _buffer[SRXL2_FRAMELEN_MAX];       /* buffer for raw srxl frame data in correct order --> buffer[0]=byte0  buffer[1]=byte1  */
    uint8_t _buflen;                          /* length in number of bytes of received srxl dataframe in buffer  */
    uint32_t _last_data_us;                   /* timespan since last received data in us   */
    uint32_t _last_run_ms;                    // last time the state machine was run
    uint16_t _channels[SRXL2_MAX_CHANNELS] = {0};    /* buffer for extracted RC channel data as pulsewidth in microseconds */

    enum {
        STATE_IDLE,                          /* do nothing */
        STATE_NEW,                           /* get header of frame + prepare for frame reception */
        STATE_COLLECT                        /* collect RC channel data from frame */
    };
    uint8_t _frame_len_full = 0U;                 /* Length in number of bytes of full srxl datastream */
    uint8_t _decode_state = STATE_IDLE;           /* Current state of SRXL frame decoding */
    uint8_t _decode_state_next = STATE_IDLE;      /* State of frame decoding that will be applied when the next byte from dataframe drops in  */
    bool _in_failsafe = false;
    int16_t _new_rssi = -1;

    SoftSerial ss{115200, SoftSerial::SERIAL_CONFIG_8N1};
};
