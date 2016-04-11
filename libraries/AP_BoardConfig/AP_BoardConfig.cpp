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
 *   AP_BoardConfig - board specific configuration
 */


#include <AP_HAL/AP_HAL.h>
#include <AP_Common/AP_Common.h>
#include "AP_BoardConfig.h"

#if CONFIG_HAL_BOARD == HAL_BOARD_PX4
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <drivers/drv_pwm_output.h>
#include <drivers/drv_sbus.h>

#ifdef CONFIG_ARCH_BOARD_PX4FMU_V1
#define BOARD_PWM_COUNT_DEFAULT 2
#define BOARD_SER1_RTSCTS_DEFAULT 0 // no flow control on UART5 on FMUv1
#else
#define BOARD_PWM_COUNT_DEFAULT 4
#define BOARD_SER1_RTSCTS_DEFAULT 2
#endif
#elif CONFIG_HAL_BOARD == HAL_BOARD_VRBRAIN

#endif

extern const AP_HAL::HAL& hal;

// table of user settable parameters
const AP_Param::GroupInfo AP_BoardConfig::var_info[] = {
#if CONFIG_HAL_BOARD == HAL_BOARD_PX4
    // @Param: PWM_COUNT
    // @DisplayName: PWM Count
    // @Description: Number of auxillary PWMs to enable. On PX4v1 only 0 or 2 is valid. On Pixhawk 0, 2, 4 or 6 is valid.
    // @Values: 0:No PWMs,2:Two PWMs,4:Four PWMs,6:Six PWMs
    AP_GROUPINFO("PWM_COUNT",    0, AP_BoardConfig, _pwm_count, BOARD_PWM_COUNT_DEFAULT),

    // @Param: SER1_RTSCTS
    // @DisplayName: Serial 1 flow control
    // @Description: Enable flow control on serial 1 (telemetry 1) on Pixhawk. You must have the RTS and CTS pins connected to your radio. The standard DF13 6 pin connector for a 3DR radio does have those pins connected. If this is set to 2 then flow control will be auto-detected by checking for the output buffer filling on startup. Note that the PX4v1 does not have hardware flow control pins on this port, so you should leave this disabled.
    // @Values: 0:Disabled,1:Enabled,2:Auto
    AP_GROUPINFO("SER1_RTSCTS",    1, AP_BoardConfig, _ser1_rtscts, BOARD_SER1_RTSCTS_DEFAULT),

    // @Param: SER2_RTSCTS
    // @DisplayName: Serial 2 flow control
    // @Description: Enable flow control on serial 2 (telemetry 2) on Pixhawk and PX4. You must have the RTS and CTS pins connected to your radio. The standard DF13 6 pin connector for a 3DR radio does have those pins connected. If this is set to 2 then flow control will be auto-detected by checking for the output buffer filling on startup.
    // @Values: 0:Disabled,1:Enabled,2:Auto
    AP_GROUPINFO("SER2_RTSCTS",    2, AP_BoardConfig, _ser2_rtscts, 2),

    // @Param: SAFETYENABLE
    // @DisplayName:  Enable use of safety arming switch
    // @Description: Disabling this option will disable the use of the safety switch on PX4 for arming. Use of the safety switch is highly recommended, so you should leave this option set to 1 except in unusual circumstances.
    // @Values: 0:Disabled,1:Enabled
    AP_GROUPINFO("SAFETYENABLE",   3, AP_BoardConfig, _safety_enable, 1),

    // @Param: SBUS_OUT
    // @DisplayName:  SBUS output rate
    // @Description: This sets the SBUS output frame rate in Hz
    // @Values: 0:Disabled,1:50Hz,2:75Hz,3:100Hz,4:150Hz,5:200Hz,6:250Hz,7:300Hz
    AP_GROUPINFO("SBUS_OUT",   4, AP_BoardConfig, _sbus_out_rate, 0),
    
#elif CONFIG_HAL_BOARD == HAL_BOARD_VRBRAIN
#endif

    // @Param: SERIAL_NUM
    // @DisplayName: User-defined serial number
    // @Description: User-defined serial number of this vehicle, it can be any arbitrary number you want and has no effect on the autopilot
    // @Range: -32767 32768
    // @User: Standard
    AP_GROUPINFO("SERIAL_NUM", 5, AP_BoardConfig, vehicleSerialNumber, 0),

#if CONFIG_HAL_BOARD == HAL_BOARD_PX4 && !defined(CONFIG_ARCH_BOARD_PX4FMU_V1)
    // @Param: CAN_ENABLE
    // @DisplayName:  Enable use of UAVCAN devices
    // @Description: Enabling this option on a Pixhawk enables UAVCAN devices. Note that this uses about 25k of memory
    // @Values: 0:Disabled,1:Enabled
    AP_GROUPINFO("CAN_ENABLE", 6, AP_BoardConfig, _can_enable, 0),
#endif
    
    AP_GROUPEND
};

#if CONFIG_HAL_BOARD == HAL_BOARD_PX4 && !defined(CONFIG_ARCH_BOARD_PX4FMU_V1)
extern "C" int uavcan_main(int argc, const char *argv[]);

#define _UAVCAN_IOCBASE             (0x4000)                        // IOCTL base for module UAVCAN
#define _UAVCAN_IOC(_n)             (_IOC(_UAVCAN_IOCBASE, _n))

#define UAVCAN_IOCG_NODEID_INPROGRESS  _UAVCAN_IOC(1)               // query if node identification is in progress

#endif

void AP_BoardConfig::init()
{
#if CONFIG_HAL_BOARD == HAL_BOARD_PX4
    /* configurre the FMU driver for the right number of PWMs */

    // ensure only valid values are set, rounding up
    if (_pwm_count > 6) _pwm_count.set(6);
    if (_pwm_count < 0) _pwm_count.set(0);
    if (_pwm_count == 1) _pwm_count.set(2);
    if (_pwm_count == 3) _pwm_count.set(4);
    if (_pwm_count == 5) _pwm_count.set(6);

    int fd = open("/dev/px4fmu", 0);
    if (fd == -1) {
        AP_HAL::panic("Unable to open /dev/px4fmu");
    }
    if (ioctl(fd, PWM_SERVO_SET_COUNT, _pwm_count.get()) != 0) {
        hal.console->printf("RCOutput: Unable to setup alt PWM to %u channels\n", _pwm_count.get());  
    }   
    close(fd);

    hal.uartC->set_flow_control((AP_HAL::UARTDriver::flow_control)_ser1_rtscts.get());
    if (hal.uartD != NULL) {
        hal.uartD->set_flow_control((AP_HAL::UARTDriver::flow_control)_ser2_rtscts.get());
    }

    if (_safety_enable.get() == 0) {
        hal.rcout->force_safety_off();
    }

    if (_sbus_out_rate.get() >= 1) {
        fd = open("/dev/px4io", 0);
        if (fd == -1) {
            hal.console->printf("SBUS: Unable to open px4io for sbus\n");
        } else {
            static const struct {
                uint8_t value;
                uint16_t rate;
            } rates[] = {
                { 1, 50 },
                { 2, 75 },
                { 3, 100 },
                { 4, 150 },
                { 5, 200 },
                { 6, 250 },
                { 7, 300 }
            };
            uint16_t rate = 300;
            for (uint8_t i=0; i<ARRAY_SIZE(rates); i++) {
                if (rates[i].value == _sbus_out_rate) {
                    rate = rates[i].rate;
                }
            }
            for (uint8_t tries=0; tries<10; tries++) {
                if (ioctl(fd, SBUS_SET_PROTO_VERSION, 1) != 0) {
                    continue;
                }
                if (ioctl(fd, PWM_SERVO_SET_SBUS_RATE, rate) != 0) {
                    continue;
                }
                break;
            }
            close(fd);
        }
    }

#if !defined(CONFIG_ARCH_BOARD_PX4FMU_V1)
    if (_can_enable >= 1) {
        const char *args[] = { "uavcan", "start", NULL };
        int ret = uavcan_main(3, args);
        if (ret != 0) {
            hal.console->printf("UAVCAN: failed to start\n");
        } else {
            hal.console->printf("UAVCAN: started\n");            
            // give some time for CAN bus initialisation
            hal.scheduler->delay(2000);
        }
    }
    if (_can_enable >= 2) {
        const char *args[] = { "uavcan", "start", "fw", NULL };
        int ret = uavcan_main(4, args);
        if (ret != 0) {
            hal.console->printf("UAVCAN: failed to start servers\n");
        } else {
            uint32_t start_wait_ms = AP_HAL::millis();
            fd = open("/dev/uavcan/esc", 0); // design flaw of uavcan driver, this should be /dev/uavcan/node one day
            if (fd == -1) {
                AP_HAL::panic("Configuration invalid - unable to open /dev/uavcan/esc");
            }

            // delay startup, UAVCAN still discovering nodes
            while (ioctl(fd, UAVCAN_IOCG_NODEID_INPROGRESS,0) == OK &&
                   AP_HAL::millis() - start_wait_ms < 7000) {
                hal.scheduler->delay(500);
            }
            hal.console->printf("UAVCAN: node discovery complete\n");
            close(fd);
        }
   }
#endif
    
#elif CONFIG_HAL_BOARD == HAL_BOARD_VRBRAIN
    /* configure the VRBRAIN driver for the right number of PWMs */

#endif    
}
