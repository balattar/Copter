#include <AP_HAL.h>

#if CONFIG_HAL_BOARD == HAL_BOARD_LINUX

#include "Scheduler.h"
#include "Storage.h"
#include <unistd.h>
#include <sys/time.h>
#include <poll.h>
#include <unistd.h>
#include <stdlib.h>

using namespace Linux;

extern const AP_HAL::HAL& hal;

LinuxScheduler::LinuxScheduler()
{}

typedef void *(*pthread_startroutine_t)(void *);

void LinuxScheduler::init(void* machtnichts)
{
    gettimeofday(&_sketch_start_time, NULL);

    pthread_attr_t thread_attr;
    struct sched_param param;

    param.sched_priority = APM_LINUX_TIMER_PRIORITY;
    (void)pthread_attr_setschedparam(&thread_attr, &param);
    pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO);

    pthread_create(&_timer_thread_ctx, &thread_attr, (pthread_startroutine_t)&Linux::LinuxScheduler::_timer_thread, this);

    // the UART thread runs at a medium priority
    pthread_attr_init(&thread_attr);
    param.sched_priority = APM_LINUX_UART_PRIORITY;
    (void)pthread_attr_setschedparam(&thread_attr, &param);
    pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO);

    pthread_create(&_uart_thread_ctx, &thread_attr, (pthread_startroutine_t)&Linux::LinuxScheduler::_uart_thread, this);
    
    // the IO thread runs at lower priority
    pthread_attr_init(&thread_attr);
    param.sched_priority = APM_LINUX_IO_PRIORITY;
    (void)pthread_attr_setschedparam(&thread_attr, &param);
    pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO);
    
    pthread_create(&_io_thread_ctx, &thread_attr, (pthread_startroutine_t)&Linux::LinuxScheduler::_io_thread, this);
}

void LinuxScheduler::delay(uint16_t ms)
{
    usleep(ms * 1000);
}

uint32_t LinuxScheduler::millis() 
{
    struct timeval tp;
    gettimeofday(&tp,NULL);
    return 1.0e3*((tp.tv_sec + (tp.tv_usec*1.0e-6)) - 
                  (_sketch_start_time.tv_sec +
                   (_sketch_start_time.tv_usec*1.0e-6)));
}

uint32_t LinuxScheduler::micros() 
{
    struct timeval tp;
    gettimeofday(&tp,NULL);
    return 1.0e6*((tp.tv_sec + (tp.tv_usec*1.0e-6)) - 
                  (_sketch_start_time.tv_sec +
                   (_sketch_start_time.tv_usec*1.0e-6)));
}

void LinuxScheduler::delay_microseconds(uint16_t us)
{
    usleep(us);
}

void LinuxScheduler::register_delay_callback(AP_HAL::Proc proc,
                                             uint16_t min_time_ms)
{
    _delay_cb = proc;
    _min_delay_cb_ms = min_time_ms;
}

void LinuxScheduler::register_timer_process(AP_HAL::TimedProc proc, void *arg) 
{
    for (uint8_t i = 0; i < _num_timer_procs; i++) {
        if (_timer_proc[i] == proc) {
            return;
        }
    }

    if (_num_timer_procs < LINUX_SCHEDULER_MAX_TIMER_PROCS) {
        _timer_proc[_num_timer_procs] = proc;
        _timer_arg[_num_timer_procs] = arg;
        _num_timer_procs++;
    } else {
        hal.console->printf("Out of timer processes\n");
    }
}

void LinuxScheduler::register_io_process(AP_HAL::TimedProc proc, void *arg) 
{
    for (uint8_t i = 0; i < _num_io_procs; i++) {
        if (_io_proc[i] == proc) {
            return;
        }
    }

    if (_num_io_procs < LINUX_SCHEDULER_MAX_TIMER_PROCS) {
        _io_proc[_num_io_procs] = proc;
        _io_arg[_num_io_procs] = arg;
        _num_io_procs++;
    } else {
        hal.console->printf("Out of IO processes\n");
    }
}

void LinuxScheduler::register_timer_failsafe(AP_HAL::TimedProc failsafe,
                                             uint32_t period_us)
{
    _failsafe = failsafe;
}

void LinuxScheduler::suspend_timer_procs()
{
    _timer_suspended = true;
}

void LinuxScheduler::resume_timer_procs()
{
    _timer_suspended = false;
    if (_timer_event_missed == true) {
        _run_timers(false);
        _timer_event_missed = false;
    }
}

void LinuxScheduler::_run_timers(bool called_from_timer_thread)
{
    uint32_t tnow = micros();
    if (_in_timer_proc) {
        return;
    }
    _in_timer_proc = true;

    if (!_timer_suspended) {
        // now call the timer based drivers
        for (int i = 0; i < _num_timer_procs; i++) {
            if (_timer_proc[i] != NULL) {
                _timer_proc[i](_timer_arg[i]);
            }
        }
    } else if (called_from_timer_thread) {
        _timer_event_missed = true;
    }

    // and the failsafe, if one is setup
    if (_failsafe != NULL) {
        _failsafe(NULL);
    }

    _in_timer_proc = false;
}

void *LinuxScheduler::_timer_thread(void)
{
    while (true) {
        poll(NULL, 0, 1);

        // run registered timers
        _run_timers(true);

    }
    return NULL;
}

void LinuxScheduler::_run_io(void)
{
    uint32_t tnow = micros();
    if (_in_io_proc) {
        return;
    }
    _in_io_proc = true;

    if (!_timer_suspended) {
        // now call the IO based drivers
        for (int i = 0; i < _num_io_procs; i++) {
            if (_io_proc[i] != NULL) {
                _io_proc[i](_io_arg[i]);
            }
        }
    }

    _in_io_proc = false;
}

void *LinuxScheduler::_uart_thread(void)
{
    while (true) {
        poll(NULL, 0, 1);

        // process any pending serial bytes: TODO
    }
    return NULL;
}

void *LinuxScheduler::_io_thread(void)
{
    while (true) {
        poll(NULL, 0, 1);

        // process any pending storage writes
        ((LinuxStorage *)hal.storage)->_timer_tick();

        // run registered IO processes
        _run_io();
    }
    return NULL;
}

void LinuxScheduler::panic(const prog_char_t *errormsg) 
{
    write(1, errormsg, strlen(errormsg));
    write(1, "\n", 1);
    hal.scheduler->delay_microseconds(10000);
    exit(1);
}

bool LinuxScheduler::in_timerprocess() 
{
    return _in_timer_proc;
}

void LinuxScheduler::begin_atomic()
{}

void LinuxScheduler::end_atomic()
{}

bool LinuxScheduler::system_initializing() {
    return !_initialized;
}

void LinuxScheduler::system_initialized()
{
    if (_initialized) {
        panic("PANIC: scheduler::system_initialized called more than once");
    }
    _initialized = true;
}

void LinuxScheduler::reboot(bool hold_in_bootloader) 
{
    for(;;);
}

#endif // CONFIG_HAL_BOARD
