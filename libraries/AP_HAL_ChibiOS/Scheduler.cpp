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

#include "AP_HAL_ChibiOS.h"
#include "Scheduler.h"
#include "Util.h"

#include <AP_HAL_ChibiOS/UARTDriver.h>
#include <AP_HAL_ChibiOS/AnalogIn.h>
#include <AP_HAL_ChibiOS/Storage.h>
#include <AP_HAL_ChibiOS/RCOutput.h>
#include <AP_HAL_ChibiOS/RCInput.h>
#include <AP_HAL_ChibiOS/CAN.h>

#include <AP_Scheduler/AP_Scheduler.h>
#include <AP_BoardConfig/AP_BoardConfig.h>
#include "shared_dma.h"
#include "sdcard.h"

#if CH_CFG_USE_DYNAMIC == TRUE

using namespace ChibiOS;

extern const AP_HAL::HAL& hal;
THD_WORKING_AREA(_timer_thread_wa, 2048);
THD_WORKING_AREA(_rcin_thread_wa, 512);
#ifdef HAL_PWM_ALARM
THD_WORKING_AREA(_toneAlarm_thread_wa, 512);
#endif
THD_WORKING_AREA(_io_thread_wa, 2048);
THD_WORKING_AREA(_storage_thread_wa, 2048);
#if HAL_WITH_UAVCAN
THD_WORKING_AREA(_uavcan_thread_wa, 4096);
#endif

Scheduler::Scheduler()
{
}

void Scheduler::init()
{
    chBSemObjectInit(&_timer_semaphore, false);
    chBSemObjectInit(&_io_semaphore, false);
    // setup the timer thread - this will call tasks at 1kHz
    _timer_thread_ctx = chThdCreateStatic(_timer_thread_wa,
                     sizeof(_timer_thread_wa),
                     APM_TIMER_PRIORITY,        /* Initial priority.    */
                     _timer_thread,             /* Thread function.     */
                     this);                     /* Thread parameter.    */

    // setup the uavcan thread - this will call tasks at 1kHz
#if HAL_WITH_UAVCAN
    _uavcan_thread_ctx = chThdCreateStatic(_uavcan_thread_wa,
                     sizeof(_uavcan_thread_wa),
                     APM_UAVCAN_PRIORITY,        /* Initial priority.    */
                     _uavcan_thread,            /* Thread function.     */
                     this);                     /* Thread parameter.    */
#endif
    // setup the RCIN thread - this will call tasks at 1kHz
    _rcin_thread_ctx = chThdCreateStatic(_rcin_thread_wa,
                     sizeof(_rcin_thread_wa),
                     APM_RCIN_PRIORITY,        /* Initial priority.    */
                     _rcin_thread,             /* Thread function.     */
                     this);                     /* Thread parameter.    */

    // the toneAlarm thread runs at a medium priority
#ifdef HAL_PWM_ALARM
    _toneAlarm_thread_ctx = chThdCreateStatic(_toneAlarm_thread_wa,
                     sizeof(_toneAlarm_thread_wa),
                     APM_TONEALARM_PRIORITY,        /* Initial priority.    */
                     _toneAlarm_thread,             /* Thread function.     */
                     this);                    /* Thread parameter.    */
#endif
    // the IO thread runs at lower priority
    _io_thread_ctx = chThdCreateStatic(_io_thread_wa,
                     sizeof(_io_thread_wa),
                     APM_IO_PRIORITY,        /* Initial priority.      */
                     _io_thread,             /* Thread function.       */
                     this);                  /* Thread parameter.      */

    // the storage thread runs at just above IO priority
    _storage_thread_ctx = chThdCreateStatic(_storage_thread_wa,
                     sizeof(_storage_thread_wa),
                     APM_STORAGE_PRIORITY,        /* Initial priority.      */
                     _storage_thread,             /* Thread function.       */
                     this);                  /* Thread parameter.      */
}


void Scheduler::delay_microseconds(uint16_t usec)
{
    if (usec == 0) { //chibios faults with 0us sleep
        return;
    }
    uint32_t ticks;
    if (usec >= 4096) {
        // we need to use 64 bit calculations for tick conversions
        ticks = US2ST64(usec);
    } else {
        ticks = US2ST(usec);
    }
    if (ticks == 0) {
        // calling with ticks == 0 causes a hard fault on ChibiOS
        ticks = 1;
    }
    chThdSleep(ticks); //Suspends Thread for desired microseconds
}

/*
  wrapper around sem_post that boosts main thread priority
 */
static void set_high_priority()
{
#if APM_MAIN_PRIORITY_BOOST != APM_MAIN_PRIORITY
    hal_chibios_set_priority(APM_MAIN_PRIORITY_BOOST);
#endif
}

/*
  return the main thread to normal priority
 */
void Scheduler::boost_end(void)
{
#if APM_MAIN_PRIORITY_BOOST != APM_MAIN_PRIORITY
    if (in_main_thread() && _priority_boosted) {
        _priority_boosted = false;
        hal_chibios_set_priority(APM_MAIN_PRIORITY);
    }
#endif
}

/*
  a variant of delay_microseconds that boosts priority to
  APM_MAIN_PRIORITY_BOOST for APM_MAIN_PRIORITY_BOOST_USEC
  microseconds when the time completes. This significantly improves
  the regularity of timing of the main loop
 */
void Scheduler::delay_microseconds_boost(uint16_t usec)
{
    if (in_main_thread()) {
        set_high_priority();
        _priority_boosted = true;
    }
    delay_microseconds(usec); //Suspends Thread for desired microseconds
    _called_boost = true;
}

/*
  return true if delay_microseconds_boost() has been called since last check
 */
bool Scheduler::check_called_boost(void)
{
    if (!_called_boost) {
        return false;
    }
    _called_boost = false;
    return true;
}

void Scheduler::delay(uint16_t ms)
{
    if (!in_main_thread()) {
        //chprintf("ERROR: delay() from timer process\n");
        return;
    }
    uint64_t start = AP_HAL::micros64();

    while ((AP_HAL::micros64() - start)/1000 < ms) {
        delay_microseconds(1000);
        if (_min_delay_cb_ms <= ms) {
            call_delay_cb();
        }
    }
}

void Scheduler::register_timer_process(AP_HAL::MemberProc proc)
{
    chBSemWait(&_timer_semaphore);
    for (uint8_t i = 0; i < _num_timer_procs; i++) {
        if (_timer_proc[i] == proc) {
            chBSemSignal(&_timer_semaphore);
            return;
        }
    }

    if (_num_timer_procs < CHIBIOS_SCHEDULER_MAX_TIMER_PROCS) {
        _timer_proc[_num_timer_procs] = proc;
        _num_timer_procs++;
    } else {
        hal.console->printf("Out of timer processes\n");
    }
    chBSemSignal(&_timer_semaphore);
}

void Scheduler::register_io_process(AP_HAL::MemberProc proc)
{
    chBSemWait(&_io_semaphore);
    for (uint8_t i = 0; i < _num_io_procs; i++) {
        if (_io_proc[i] == proc) {
            chBSemSignal(&_io_semaphore);
            return;
        }
    }
    
    if (_num_io_procs < CHIBIOS_SCHEDULER_MAX_TIMER_PROCS) {
        _io_proc[_num_io_procs] = proc;
        _num_io_procs++;
    } else {
        hal.console->printf("Out of IO processes\n");
    }
    chBSemSignal(&_io_semaphore);
}

void Scheduler::register_timer_failsafe(AP_HAL::Proc failsafe, uint32_t period_us)
{
    _failsafe = failsafe;
}

extern void Reset_Handler();
void Scheduler::reboot(bool hold_in_bootloader)
{
    // disarm motors to ensure they are off during a bootloader upload
    hal.rcout->force_safety_on();
    hal.rcout->force_safety_no_wait();

    // stop sdcard driver, if active
    sdcard_stop();

    // disable all interrupt sources
    port_disable();
    
    // reboot
    NVIC_SystemReset();
}

void Scheduler::_run_timers()
{
    if (_in_timer_proc) {
        return;
    }
    _in_timer_proc = true;

    int num_procs = 0;
    chBSemWait(&_timer_semaphore);
    num_procs = _num_timer_procs;
    chBSemSignal(&_timer_semaphore);
    // now call the timer based drivers
    for (int i = 0; i < num_procs; i++) {
        if (_timer_proc[i]) {
            _timer_proc[i]();
        }
    }

    // and the failsafe, if one is setup
    if (_failsafe != nullptr) {
        _failsafe();
    }

#if HAL_USE_ADC == TRUE
    // process analog input
    ((AnalogIn *)hal.analogin)->_timer_tick();
#endif

    _in_timer_proc = false;
}

void Scheduler::_timer_thread(void *arg)
{
    Scheduler *sched = (Scheduler *)arg;
    chRegSetThreadName("apm_timer");

    while (!sched->_hal_initialized) {
        sched->delay_microseconds(1000);
    }
    while (true) {
        sched->delay_microseconds(1000);

        // run registered timers
        sched->_run_timers();

        // process any pending RC output requests
        hal.rcout->timer_tick();
    }
}
#if HAL_WITH_UAVCAN
void Scheduler::_uavcan_thread(void *arg)
{
    Scheduler *sched = (Scheduler *)arg;
    chRegSetThreadName("apm_uavcan");
    while (!sched->_hal_initialized) {
        sched->delay_microseconds(20000);
    }
    while (true) {
        sched->delay_microseconds(300);
        for (int i = 0; i < MAX_NUMBER_OF_CAN_INTERFACES; i++) {
            if (AP_UAVCAN::get_uavcan(i) != nullptr) {
                CANManager::from(hal.can_mgr[i])->_timer_tick();
            }
        }
    }
}
#endif

void Scheduler::_rcin_thread(void *arg)
{
    Scheduler *sched = (Scheduler *)arg;
    chRegSetThreadName("apm_rcin");
    while (!sched->_hal_initialized) {
        sched->delay_microseconds(20000);
    }
    while (true) {
        sched->delay_microseconds(2500);
        ((RCInput *)hal.rcin)->_timer_tick();
    }
}
#ifdef HAL_PWM_ALARM

void Scheduler::_toneAlarm_thread(void *arg)
{
    Scheduler *sched = (Scheduler *)arg;
    chRegSetThreadName("toneAlarm");
    while (!sched->_hal_initialized) {
        sched->delay_microseconds(20000);
    }
    while (true) {
        sched->delay_microseconds(20000);

        // process tone command
        Util::from(hal.util)->_toneAlarm_timer_tick();
    }
}
#endif
void Scheduler::_run_io(void)
{
    if (_in_io_proc) {
        return;
    }
    _in_io_proc = true;

    int num_procs = 0;
    chBSemWait(&_io_semaphore);
    num_procs = _num_io_procs;
    chBSemSignal(&_io_semaphore);
    // now call the IO based drivers
    for (int i = 0; i < num_procs; i++) {
        if (_io_proc[i]) {
            _io_proc[i]();
        }
    }

    _in_io_proc = false;
}

void Scheduler::_io_thread(void* arg)
{
    Scheduler *sched = (Scheduler *)arg;
    chRegSetThreadName("apm_io");
    while (!sched->_hal_initialized) {
        sched->delay_microseconds(1000);
    }
    while (true) {
        sched->delay_microseconds(1000);

        // run registered IO processes
        sched->_run_io();
    }
}

void Scheduler::_storage_thread(void* arg)
{
    Scheduler *sched = (Scheduler *)arg;
    chRegSetThreadName("apm_storage");
    while (!sched->_hal_initialized) {
        sched->delay_microseconds(10000);
    }
    while (true) {
        sched->delay_microseconds(10000);

        // process any pending storage writes
        hal.storage->_timer_tick();
    }
}

bool Scheduler::in_main_thread() const
{
    return get_main_thread() == chThdGetSelfX();
}

void Scheduler::system_initialized()
{
    if (_initialized) {
        AP_HAL::panic("PANIC: scheduler::system_initialized called"
                      "more than once");
    }
    _initialized = true;
}

/*
  disable interrupts and return a context that can be used to
  restore the interrupt state. This can be used to protect
  critical regions
*/
void *Scheduler::disable_interrupts_save(void)
{
    return (void *)(uintptr_t)chSysGetStatusAndLockX();
}

/*
  restore interrupt state from disable_interrupts_save()
*/
void Scheduler::restore_interrupts(void *state)
{
    chSysRestoreStatusX((syssts_t)(uintptr_t)state);
}

#endif // CH_CFG_USE_DYNAMIC
