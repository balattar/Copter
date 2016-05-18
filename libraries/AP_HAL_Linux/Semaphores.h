#pragma once

#include <AP_HAL/AP_HAL_Boards.h>

#include "AP_HAL_Linux.h"
#include <pthread.h>

class Linux::Semaphore : public AP_HAL::Semaphore {
public:
    Semaphore() {
        pthread_mutex_init(&_lock, NULL);
    }
    bool give();
    bool take(uint32_t timeout_ms);
    bool take_nonblocking();
private:
    pthread_mutex_t _lock;
};
