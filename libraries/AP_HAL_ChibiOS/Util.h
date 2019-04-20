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
#pragma once

#include <AP_HAL/AP_HAL.h>
#include "AP_HAL_ChibiOS_Namespace.h"
#include "AP_HAL_ChibiOS.h"
#include <ch.h>

class ChibiOS::Util : public AP_HAL::Util {
public:
    static Util *from(AP_HAL::Util *util) {
        return static_cast<Util*>(util);
    }

    bool run_debug_shell(AP_HAL::BetterStream *stream) override { return false; }
    uint32_t available_memory() override;

    // Special Allocation Routines
    void *malloc_type(size_t size, AP_HAL::Util::Memory_Type mem_type) override;
    void free_type(void *ptr, size_t size, AP_HAL::Util::Memory_Type mem_type) override;

#ifdef ENABLE_HEAP
    // heap functions, note that a heap once alloc'd cannot be dealloc'd
    virtual void *allocate_heap_memory(size_t size);
    virtual void *heap_realloc(void *heap, void *ptr, size_t new_size);
#endif // ENABLE_HEAP

    /*
      return state of safety switch, if applicable
     */
    enum safety_state safety_switch_state(void) override;

    // IMU temperature control
    void set_imu_temp(float current) override;
    void set_imu_target_temp(int8_t *target) override;

    // get system ID as a string
    bool get_system_id(char buf[40]) override;
    bool get_system_id_unformatted(uint8_t buf[], uint8_t &len) override;

#ifdef HAL_PWM_ALARM
    bool toneAlarm_init() override;
    void toneAlarm_set_buzzer_tone(float frequency, float volume, uint32_t duration_ms) override;
#endif

#ifdef USE_POSIX
    /*
      initialise (or re-initialise) filesystem storage
     */
    bool fs_init(void) override;
#endif

    // return true if the reason for the reboot was a watchdog reset
    bool was_watchdog_reset() const override;

    // return true if safety was off and this was a watchdog reset
    bool was_watchdog_safety_off() const override;

    // return true if vehicle was armed and this was a watchdog reset
    bool was_watchdog_armed() const override;

    // backup home state for restore on watchdog reset
    void set_backup_home_state(int32_t lat, int32_t lon, int32_t alt_cm) const override;

    // backup home state for restore on watchdog reset
    bool get_backup_home_state(int32_t &lat, int32_t &lon, int32_t &alt_cm) const override;
    
private:
#ifdef HAL_PWM_ALARM
    struct ToneAlarmPwmGroup {
        pwmchannel_t chan;
        PWMConfig pwm_cfg;
        PWMDriver* pwm_drv;
    };

    static ToneAlarmPwmGroup _toneAlarm_pwm_group;
#endif

#if HAL_HAVE_IMU_HEATER
    struct {
        int8_t *target;
        float integrator;
        uint16_t count;
        float sum;
        uint32_t last_update_ms;
        uint8_t duty_counter;
        float output;
    } heater;
#endif

    /*
      set HW RTC in UTC microseconds
     */
    void set_hw_rtc(uint64_t time_utc_usec) override;

    /*
      get system clock in UTC microseconds
     */
    uint64_t get_hw_rtc() const override;
#ifndef HAL_NO_FLASH_SUPPORT
    bool flash_bootloader() override;
#endif

#ifdef ENABLE_HEAP
    static memory_heap_t scripting_heap;
#endif // ENABLE_HEAP

    void set_soft_armed(const bool b) override;

};
