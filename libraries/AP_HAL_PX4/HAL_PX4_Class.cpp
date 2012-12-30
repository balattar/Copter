/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <AP_HAL.h>

#if CONFIG_HAL_BOARD == HAL_BOARD_PX4

#include <AP_HAL_PX4.h>
#include "AP_HAL_PX4_Namespace.h"
#include "HAL_PX4_Class.h"

#include <AP_HAL_Empty.h>
#include <AP_HAL_Empty_Private.h>

using namespace PX4;

static Empty::EmptyUARTDriver uartADriver;
static Empty::EmptyUARTDriver uartBDriver;
static Empty::EmptyUARTDriver uartCDriver;
static Empty::EmptyI2CDriver  i2cDriver;
static Empty::EmptySPIDeviceManager spiDeviceManager;
static Empty::EmptyAnalogIn analogIn;
static Empty::EmptyStorage storageDriver;
static Empty::EmptyGPIO gpioDriver;
static Empty::EmptyRCInput rcinDriver;
static Empty::EmptyRCOutput rcoutDriver;
static Empty::EmptyScheduler schedulerInstance;
static Empty::EmptyUtil utilInstance;

static Empty::EmptyConsoleDriver consoleDriver(&uartADriver);

HAL_PX4::HAL_PX4() :
    AP_HAL::HAL(
	    &uartADriver,  /* uartA */
	    &uartBDriver,  /* uartB */
	    &uartCDriver,  /* uartC */
        &i2cDriver, /* i2c */
        &spiDeviceManager, /* spi */
        &analogIn, /* analogin */
        &storageDriver, /* storage */
        &consoleDriver, /* console */
        &gpioDriver, /* gpio */
        &rcinDriver,  /* rcinput */
        &rcoutDriver, /* rcoutput */
        &schedulerInstance, /* scheduler */
        &utilInstance) /* util */
{}

void HAL_PX4::init(int argc, char * const argv[]) const 
{
    scheduler->init(NULL);
    uartA->begin(115200);
    console->init((void*) uartA);
    rcin->init(NULL);
    rcout->init(NULL);
    analogin->init(NULL);
}

const HAL_PX4 AP_HAL_PX4;

#endif // CONFIG_HAL_BOARD == HAL_BOARD_PX4

