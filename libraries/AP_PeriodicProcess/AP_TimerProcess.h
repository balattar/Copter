
#ifndef __AP_TIMERPROCESS_H__
#define __AP_TIMERPROCESS_H__

#include "PeriodicProcess.h"
#include "../Arduino_Mega_ISR_Registry/Arduino_Mega_ISR_Registry.h"

// default to 1kHz timer interrupt
#define TIMERPROCESS_PER_DEFAULT (256-62) // 1kHz

#define AP_TIMERPROCESS_MAX_PROCS 5

class AP_TimerProcess : public AP_PeriodicProcess
{
public:
    AP_TimerProcess(uint8_t period = TIMERPROCESS_PER_DEFAULT);
    void                        init( Arduino_Mega_ISR_Registry * isr_reg );
    void                        register_process(ap_procedure proc);
    bool                        queue_process(ap_procedure proc);   // queue process to run as soon as possible after any currently running ap_processes complete.  returns true if it ran immediately
    void                        set_failsafe(ap_procedure proc);
    void                        suspend_timer(void);
    void                        resume_timer(void);
    bool                        running();
    static void                 run(void);
protected:
    static uint8_t              _period;
    static ap_procedure         _proc[AP_TIMERPROCESS_MAX_PROCS];
    static ap_procedure         _failsafe;
    static ap_procedure         _queued_proc;
    static uint8_t              _pidx;
    static bool                 _in_timer_call;
    static bool                 _suspended;
};

#endif // __AP_TIMERPROCESS_H__
