/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
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
 *       AP_Compass_PX4.cpp - Arduino Library for PX4 magnetometer
 *
 */


#include <AP_HAL.h>

#if CONFIG_HAL_BOARD == HAL_BOARD_PX4
#include "AP_Compass_PX4.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <drivers/drv_mag.h>
#include <drivers/drv_hrt.h>
#include <stdio.h>
#include <errno.h>

extern const AP_HAL::HAL& hal;


// Public Methods //////////////////////////////////////////////////////////////

bool AP_Compass_PX4::init(void)
{
	_mag_fd = open(MAG_DEVICE_PATH, O_RDONLY);
	if (_mag_fd < 0) {
        hal.console->printf("Unable to open " MAG_DEVICE_PATH);
        return false;
	}

	/* set the mag internal poll rate to at least 150Hz */
	ioctl(_mag_fd, MAGIOCSSAMPLERATE, 150);

	/* set the driver to poll at 150Hz */
	ioctl(_mag_fd, SENSORIOCSPOLLRATE, 150);

    // average over up to 20 samples
    ioctl(_mag_fd, SENSORIOCSQUEUEDEPTH, 20);

    healthy = false;
    _count = 0;
    _sum.zero();

    // give the driver a chance to run, and gather one sample
    hal.scheduler->delay(40);
    accumulate();

    return true;
}

bool AP_Compass_PX4::read(void)
{
    // try to accumulate one more sample, so we have the latest data
    accumulate();

    // consider the compass healthy if we got a reading in the last 0.2s
    healthy = (hrt_absolute_time() - _last_timestamp < 200000);
    if (!healthy || _count == 0) {
        return healthy;
    }

    _sum /= _count;
    _sum *= 1000;

    // apply default board orientation for this compass type. This is
    // a noop on most boards
    _sum.rotate(MAG_BOARD_ORIENTATION);

    // add user selectable orientation
    _sum.rotate((enum Rotation)_orientation.get());

    // and add in AHRS_ORIENTATION setting
    _sum.rotate(_board_orientation);
    _sum += _offset.get();

    // apply motor compensation
    if (_motor_comp_type != AP_COMPASS_MOT_COMP_DISABLED && _thr_or_curr != 0.0f) {
        _motor_offset = _motor_compensation.get() * _thr_or_curr;
        _sum += _motor_offset;
    } else {
        _motor_offset.x = 0;
        _motor_offset.y = 0;
        _motor_offset.z = 0;
    }
    
    mag_x = _sum.x;
    mag_y = _sum.y;
    mag_z = _sum.z;
    
    _sum.zero();
    _count = 0;

    last_update = _last_timestamp;
    
    return true;
}

void AP_Compass_PX4::accumulate(void)
{
    struct mag_report mag_report;
    while (::read(_mag_fd, &mag_report, sizeof(mag_report)) == sizeof(mag_report) &&
           mag_report.timestamp != _last_timestamp) {
        _sum += Vector3f(mag_report.x, mag_report.y, mag_report.z);
        _count++;
        _last_timestamp = mag_report.timestamp;
    }
}

#endif // CONFIG_HAL_BOARD
