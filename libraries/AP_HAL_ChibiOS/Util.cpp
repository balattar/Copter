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
 * Code by Andrew Tridgell and Siddharth Bharat Purohit
 */
#include <AP_HAL/AP_HAL.h>
#include <AP_Math/AP_Math.h>

#if CONFIG_HAL_BOARD == HAL_BOARD_CHIBIOS
#include "Util.h"
#include <chheap.h>

#if HAL_WITH_IO_MCU
#include <AP_BoardConfig/AP_BoardConfig.h>
#include <AP_IOMCU/AP_IOMCU.h>
extern AP_IOMCU iomcu;
#endif

using namespace ChibiOS;

extern "C" {
    size_t mem_available(void);
    void *malloc_ccm(size_t size);
};

/**
   how much free memory do we have in bytes.
*/
uint32_t ChibiUtil::available_memory(void)
{
    // from malloc.c in hwdef
    return mem_available();
}

/*
    Special Allocation Routines
*/

void* ChibiUtil::malloc_type(size_t size, AP_HAL::Util::Memory_Type mem_type)
{
    if (mem_type == AP_HAL::Util::MEM_FAST) {
        return try_alloc_from_ccm_ram(size);
    } else {
        return malloc(size);
    }
}

void ChibiUtil::free_type(void *ptr, size_t size, AP_HAL::Util::Memory_Type mem_type)
{
    if (ptr != NULL) {
        chHeapFree(ptr);
    }
}


void* ChibiUtil::try_alloc_from_ccm_ram(size_t size)
{
    void *ret = malloc_ccm(size);
    if (ret == nullptr) {
        //we failed to allocate from CCM so we are going to try common SRAM
        ret = malloc(size);
    }
    return ret;
}

/*
  get safety switch state
 */
ChibiUtil::safety_state ChibiUtil::safety_switch_state(void)
{
#if HAL_WITH_IO_MCU
    if (AP_BoardConfig::io_enabled()) {
        return iomcu.get_safety_switch_state();
    }
#endif
    return SAFETY_NONE;
}

void ChibiUtil::set_imu_temp(float current)
{
#if HAL_WITH_IO_MCU && HAL_HAVE_IMU_HEATER
    if (!heater.target || *heater.target == -1 || !AP_BoardConfig::io_enabled()) {
        return;
    }

    // average over temperatures to remove noise
    heater.count++;
    heater.sum += current;
    
    // update once a second
    uint32_t now = AP_HAL::millis();
    if (now - heater.last_update_ms < 1000) {
        return;
    }
    heater.last_update_ms = now;

    current = heater.sum / heater.count;
    heater.sum = 0;
    heater.count = 0;

    // experimentally tweaked for Pixhawk2
    const float kI = 0.3f;
    const float kP = 200.0f;
    float target = (float)(*heater.target);

    // limit to 65 degrees to prevent damage
    target = constrain_float(target, 0, 65);
    
    float err = target - current;

    heater.integrator += kI * err;
    heater.integrator = constrain_float(heater.integrator, 0, 70);

    float output = constrain_float(kP * err + heater.integrator, 0, 100);
    
    // hal.console->printf("integrator %.1f out=%.1f temp=%.2f err=%.2f\n", heater.integrator, output, current, err);

    iomcu.set_heater_duty_cycle(output);
#endif // HAL_WITH_IO_MCU && HAL_HAVE_IMU_HEATER
}

void ChibiUtil::set_imu_target_temp(int8_t *target)
{
#if HAL_WITH_IO_MCU && HAL_HAVE_IMU_HEATER
    heater.target = target;
#endif
}

#endif //CONFIG_HAL_BOARD == HAL_BOARD_CHIBIOS
