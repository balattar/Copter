// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

/// @file	AP_GPS_Auto.cpp
/// @brief	Simple GPS auto-detection logic.

#include <AP_HAL.h>
#include <AP_Common.h>

#include "AP_GPS.h"             // includes AP_GPS_Auto.h

extern const AP_HAL::HAL& hal;

static const uint32_t baudrates[] PROGMEM = {38400U, 57600U, 9600U, 4800U};

const prog_char AP_GPS_Auto::_ublox_set_binary[] PROGMEM = UBLOX_SET_BINARY;
const prog_char AP_GPS_Auto::_mtk_set_binary[]   PROGMEM = MTK_SET_BINARY;
const prog_char AP_GPS_Auto::_sirf_set_binary[]  PROGMEM = SIRF_SET_BINARY;


AP_GPS_Auto::AP_GPS_Auto(GPS **gps)  :
	GPS(),
    _gps(gps),
    state(NULL)
{
}

// Do nothing at init time - it may be too early to try detecting the GPS
void
AP_GPS_Auto::init(AP_HAL::UARTDriver *s, enum GPS_Engine_Setting nav_setting, DataFlash_Class *DataFlash)
{
    if (state == NULL) {
        state = (struct detect_state *)calloc(1, sizeof(*state));
    }
	_port = s;
    _nav_setting = nav_setting;
    _DataFlash = DataFlash;
}


// Called the first time that a client tries to kick the GPS to update.
//
// We detect the real GPS, then update the pointer we have been called through
// and return.
//
bool
AP_GPS_Auto::read(void)
{
    if (state == NULL) {
        return false;
    }
	GPS *gps;
	uint32_t now = hal.scheduler->millis();

	if (now - state->last_baud_change_ms > 1200) {
		// its been more than 1.2 seconds without detection on this
		// GPS - switch to another baud rate
		_baudrate = pgm_read_dword(&baudrates[state->last_baud]);
		//hal.console->printf_P(PSTR("Setting GPS baudrate %u\n"), (unsigned)_baudrate);
		_port->begin(_baudrate, 256, 16);		
		state->last_baud++;
		state->last_baud_change_ms = now;
		if (state->last_baud == sizeof(baudrates) / sizeof(baudrates[0])) {
			state->last_baud = 0;
		}
		// write config strings for the types of GPS we support
		_send_progstr(_mtk_set_binary, sizeof(_mtk_set_binary));
		_send_progstr(_ublox_set_binary, sizeof(_ublox_set_binary));
		_send_progstr(_sirf_set_binary, sizeof(_sirf_set_binary));
	}

	_update_progstr();

	if (NULL != (gps = _detect())) {
		// configure the detected GPS
		gps->init(_port, _nav_setting, _DataFlash);
		hal.console->println_P(PSTR("OK"));
        free(state);
        state = NULL;
		*_gps = gps;
		return true;
    }
    return false;
}

//
// Perform one iteration of the auto-detection process.
//
GPS *
AP_GPS_Auto::_detect(void)
{
	GPS *new_gps = NULL;

	if (state->detect_started_ms == 0 && _port->available() > 0) {
		state->detect_started_ms = hal.scheduler->millis();
	}

	while (_port->available() > 0 && new_gps == NULL) {
		uint8_t data = _port->read();
		/*
		  running a uBlox at less than 38400 will lead to packet
		  corruption, as we can't receive the packets in the 200ms
		  window for 5Hz fixes. The NMEA startup message should force
		  the uBlox into 38400 no matter what rate it is configured
		  for.
		 */
		if (_baudrate >= 38400 && AP_GPS_UBLOX::_detect(state->ublox_detect_state, data)) {
			hal.console->print_P(PSTR(" ublox "));
			new_gps = new AP_GPS_UBLOX();
		} 
		else if (AP_GPS_MTK19::_detect(state->mtk19_detect_state, data)) {
			hal.console->print_P(PSTR(" MTK19 "));
			new_gps = new AP_GPS_MTK19();
		} 
		else if (AP_GPS_MTK::_detect(state->mtk_detect_state, data)) {
			hal.console->print_P(PSTR(" MTK "));
			new_gps = new AP_GPS_MTK();
		}
#if !defined( __AVR_ATmega1280__ )
		// save a bit of code space on a 1280
		else if (AP_GPS_SIRF::_detect(state->sirf_detect_state, data)) {
			hal.console->print_P(PSTR(" SIRF "));
			new_gps = new AP_GPS_SIRF();
		}
		else if (hal.scheduler->millis() - state->detect_started_ms > 5000) {
			// prevent false detection of NMEA mode in
			// a MTK or UBLOX which has booted in NMEA mode
			if (AP_GPS_NMEA::_detect(state->nmea_detect_state, data)) {
				hal.console->print_P(PSTR(" NMEA "));
				new_gps = new AP_GPS_NMEA();
			}
		}
#endif
	}

	if (new_gps != NULL) {
		new_gps->init(_port, _nav_setting, _DataFlash);
	}

	return new_gps;
}


/*
  a prog_char block queue, used to send out config commands to a GPS
  in 16 byte chunks. This saves us having to have a 128 byte GPS send
  buffer, while allowing us to avoid a long delay in sending GPS init
  strings while waiting for the GPS auto detection to happen
 */

void AP_GPS_Auto::_send_progstr(const prog_char *pstr, uint8_t size)
{
	struct progstr_queue *q = &state->progstr_state.queue[state->progstr_state.next_idx];
	q->pstr = pstr;
	q->size = size;
	q->ofs = 0;
	state->progstr_state.next_idx++;
	if (state->progstr_state.next_idx == PROGSTR_QUEUE_SIZE) {
		state->progstr_state.next_idx = 0;
	}
}

void AP_GPS_Auto::_update_progstr(void)
{
	struct progstr_queue *q = &state->progstr_state.queue[state->progstr_state.idx];
	// quick return if nothing to do
	if (q->size == 0 || _port->tx_pending()) {
		return;
	}
	uint8_t nbytes = q->size - q->ofs;
	if (nbytes > 16) {
		nbytes = 16;
	}
	//hal.console->printf_P(PSTR("writing %u bytes\n"), (unsigned)nbytes);
	_write_progstr_block(_port, q->pstr+q->ofs, nbytes);
	q->ofs += nbytes;
	if (q->ofs == q->size) {
		q->size = 0;
		state->progstr_state.idx++;
		if (state->progstr_state.idx == PROGSTR_QUEUE_SIZE) {
			state->progstr_state.idx = 0;
		}
	}
}

