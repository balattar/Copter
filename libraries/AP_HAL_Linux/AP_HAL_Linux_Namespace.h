#pragma once

namespace Linux {
    class UARTDriver;
    class SPIUARTDriver;
    class RPIOUARTDriver;
    class I2CDevice;
    class I2CDeviceManager;
    class SPIDeviceManager;
    class SPIDeviceDriver;
    class Storage;
    class GPIO_BBB;
    class GPIO_RPI;
    class GPIO_Sysfs;
    class Storage;
    class DigitalSource;
    class DigitalSource_Sysfs;
    class PeriodicThread;
    class PWM_Sysfs;
    class PWM_Sysfs_Bebop;
    class PWM_Sysfs_Base;
    class RCInput;
    class RCInput_PRU;
    class RCInput_AioPRU;
    class RCInput_RPI;
    class RCInput_Raspilot;
    class RCInput_Navio2;
    class RCInput_ZYNQ;
    class RCInput_UART;
    class RCInput_UDP;
    class RCInput_DSM;
    class RCInput_SBUS;
    class RCOutput_PRU;
    class RCOutput_AioPRU;
    class RCOutput_PCA9685;
    class RCOutput_Raspilot;
    class RCOutput_ZYNQ;
    class RCOutput_Bebop;
    class RCOutput_Sysfs;
    class RCOutput_QFLIGHT;
    class RCOutput_Disco;
    class Semaphore;
    class Scheduler;
    class Util;
    class UtilRPI;
    class ToneAlarm;
    class ToneAlarm_Raspilot;
    class Thread;
    class Heat;
    class HeatPwm;
    class CameraSensor;
    class CameraSensor_Mt9v117;
    class VideoIn;
    class OpticalFlow_Onboard;
    class Flow_PX4;
}
