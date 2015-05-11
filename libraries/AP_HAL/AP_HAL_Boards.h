
#ifndef __AP_HAL_BOARDS_H__
#define __AP_HAL_BOARDS_H__

/**
 * C preprocesor enumeration of the boards supported by the AP_HAL.
 * This list exists so HAL_BOARD == HAL_BOARD_xxx preprocessor blocks
 * can be used to exclude HAL boards from the build when appropriate.
 * Its not an elegant solution but we can improve it in future.
 */

#define HAL_BOARD_SITL     3
#define HAL_BOARD_SMACCM   4 // unused
#define HAL_BOARD_PX4      5
#define HAL_BOARD_FLYMAPLE 6
#define HAL_BOARD_LINUX    7
#define HAL_BOARD_VRBRAIN  8
#define HAL_BOARD_QURT     9
#define HAL_BOARD_EMPTY    99

// default board subtype is -1
#define HAL_BOARD_SUBTYPE_NONE -1

/**
   HAL Linux sub-types, starting at 1000
 */
#define HAL_BOARD_SUBTYPE_LINUX_NONE     1000
#define HAL_BOARD_SUBTYPE_LINUX_ERLEBOARD 1001
#define HAL_BOARD_SUBTYPE_LINUX_PXF      1002
#define HAL_BOARD_SUBTYPE_LINUX_NAVIO    1003
#define HAL_BOARD_SUBTYPE_LINUX_ZYNQ     1004
#define HAL_BOARD_SUBTYPE_LINUX_BBBMINI  1005
#define HAL_BOARD_SUBTYPE_LINUX_BEBOP    1006
#define HAL_BOARD_SUBTYPE_LINUX_RASPILOT 1007
#define HAL_BOARD_SUBTYPE_LINUX_MINLURE  1008
#define HAL_BOARD_SUBTYPE_LINUX_ERLEBRAIN2 1009
#define HAL_BOARD_SUBTYPE_LINUX_BH       1010
#define HAL_BOARD_SUBTYPE_LINUX_QFLIGHT  1011
#define HAL_BOARD_SUBTYPE_LINUX_PXFMINI  1012

/**
   HAL PX4 sub-types, starting at 2000
 */
#define HAL_BOARD_SUBTYPE_PX4_V1         2000
#define HAL_BOARD_SUBTYPE_PX4_V2         2001

/**
   HAL VRBRAIN sub-types, starting at 4000
 */
#define HAL_BOARD_SUBTYPE_VRBRAIN_V45    4000
#define HAL_BOARD_SUBTYPE_VRBRAIN_V51    4001
#define HAL_BOARD_SUBTYPE_VRBRAIN_V52    4002
#define HAL_BOARD_SUBTYPE_VRUBRAIN_V51   4003
#define HAL_BOARD_SUBTYPE_VRUBRAIN_V52   4004

// InertialSensor driver types
#define HAL_INS_MPU60XX_SPI 2
#define HAL_INS_MPU60XX_I2C 3
#define HAL_INS_HIL     4
#define HAL_INS_PX4     5
#define HAL_INS_FLYMAPLE 6
#define HAL_INS_L3G4200D 7
#define HAL_INS_VRBRAIN  8
#define HAL_INS_MPU9250  9
#define HAL_INS_L3GD20   10
#define HAL_INS_LSM9DS0 11
#define HAL_INS_RASPILOT 12
#define HAL_INS_MPU9250_I2C 13
#define HAL_INS_BH          14
#define HAL_INS_QFLIGHT  15
#define HAL_INS_QURT     16

// barometer driver types
#define HAL_BARO_BMP085     1
#define HAL_BARO_MS5611     2
#define HAL_BARO_MS5611_SPI 3
#define HAL_BARO_MS5607     4
#define HAL_BARO_PX4        5
#define HAL_BARO_HIL        6
#define HAL_BARO_VRBRAIN    7
#define HAL_BARO_MS5637_I2C 8
#define HAL_BARO_QFLIGHT    9
#define HAL_BARO_QURT      10

// compass driver types
#define HAL_COMPASS_HMC5843   1
#define HAL_COMPASS_PX4       2
#define HAL_COMPASS_HIL       3
#define HAL_COMPASS_VRBRAIN   4
#define HAL_COMPASS_AK8963_MPU9250 5
#define HAL_COMPASS_AK8963_I2C  6
#define HAL_COMPASS_HMC5843_MPU6000 7
#define HAL_COMPASS_RASPILOT  8
#define HAL_COMPASS_AK8963_MPU9250_I2C  9
#define HAL_COMPASS_BH                  10
#define HAL_COMPASS_QFLIGHT   11
#define HAL_COMPASS_QURT      12

// Heat Types
#define HAL_LINUX_HEAT_PWM 1

/**
   CPU classes, used to select if CPU intensive algorithms should be used

   Note that these are only approximate, not exact CPU speeds.
 */
#define HAL_CPU_CLASS_16   1   // DEPRECATED: 16Mhz, AVR2560 or similar
#define HAL_CPU_CLASS_75   2   // 75Mhz, Flymaple or similar
#define HAL_CPU_CLASS_150  3   // 150Mhz, PX4 or similar, assumes
                               // hardware floating point. Assumes tens
                               // of kilobytes of memory available
#define HAL_CPU_CLASS_1000 4   // GigaHz class, SITL, BeagleBone etc,
                               // assumes megabytes of memory available

/**
   operating system features:

   HAL implementations may define the following extra feature defines
   to 1 if available

  HAL_OS_POSIX_IO    :  has posix-like filesystem IO
  HAL_OS_SOCKETS     :  has posix-like sockets
 */

#if CONFIG_HAL_BOARD == HAL_BOARD_SITL
#define HAL_BOARD_NAME "SITL"
#define HAL_CPU_CLASS HAL_CPU_CLASS_1000
#define HAL_OS_POSIX_IO 1
#define HAL_OS_SOCKETS 1
#define HAL_STORAGE_SIZE            16384
#define HAL_STORAGE_SIZE_AVAILABLE  HAL_STORAGE_SIZE
#define HAL_BOARD_LOG_DIRECTORY "logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "terrain"
#define HAL_PARAM_DEFAULTS_PATH "etc/defaults.parm"
#define HAL_INS_DEFAULT HAL_INS_HIL
#define HAL_BARO_DEFAULT HAL_BARO_HIL
#define HAL_COMPASS_DEFAULT HAL_COMPASS_HIL
#define HAL_GPIO_A_LED_PIN        61
#define HAL_GPIO_B_LED_PIN        48
#define HAL_GPIO_C_LED_PIN        117
#define HAL_GPIO_LED_ON           LOW
#define HAL_GPIO_LED_OFF          HIGH

#elif CONFIG_HAL_BOARD == HAL_BOARD_FLYMAPLE
#define HAL_BOARD_NAME "FLYMAPLE"
#define HAL_CPU_CLASS HAL_CPU_CLASS_75
#define HAL_STORAGE_SIZE            4096
#define HAL_STORAGE_SIZE_AVAILABLE  HAL_STORAGE_SIZE
#define HAL_INS_DEFAULT HAL_INS_FLYMAPLE
#define HAL_BARO_DEFAULT HAL_BARO_BMP085
#define HAL_COMPASS_DEFAULT HAL_COMPASS_HMC5843
#define HAL_SERIAL0_BAUD_DEFAULT 115200
#define CONFIG_HAL_BOARD_SUBTYPE HAL_BOARD_SUBTYPE_NONE
#define HAL_GPIO_A_LED_PIN        13
#define HAL_GPIO_B_LED_PIN        13
#define HAL_GPIO_C_LED_PIN        13
#define HAL_GPIO_LED_ON           LOW
#define HAL_GPIO_LED_OFF          HIGH

#elif CONFIG_HAL_BOARD == HAL_BOARD_PX4
#define HAL_BOARD_NAME "PX4"
#define HAL_CPU_CLASS HAL_CPU_CLASS_150
#define HAL_OS_POSIX_IO 1
#define HAL_BOARD_LOG_DIRECTORY "/fs/microsd/APM/LOGS"
#define HAL_BOARD_TERRAIN_DIRECTORY "/fs/microsd/APM/TERRAIN"
#define HAL_PARAM_DEFAULTS_PATH "/etc/defaults.parm"
#define HAL_INS_DEFAULT HAL_INS_PX4
#define HAL_BARO_DEFAULT HAL_BARO_PX4
#define HAL_COMPASS_DEFAULT HAL_COMPASS_PX4
#define HAL_SERIAL0_BAUD_DEFAULT 115200
#ifdef CONFIG_ARCH_BOARD_PX4FMU_V1
#define CONFIG_HAL_BOARD_SUBTYPE HAL_BOARD_SUBTYPE_PX4_V1
#define HAL_STORAGE_SIZE            8192
#else
#define CONFIG_HAL_BOARD_SUBTYPE HAL_BOARD_SUBTYPE_PX4_V2
#define HAL_STORAGE_SIZE            16384
#endif
#define HAL_GPIO_A_LED_PIN        27
#define HAL_GPIO_B_LED_PIN        26
#define HAL_GPIO_C_LED_PIN        25
#define HAL_GPIO_LED_ON           LOW
#define HAL_GPIO_LED_OFF          HIGH

#elif CONFIG_HAL_BOARD == HAL_BOARD_LINUX
#define HAL_BOARD_NAME "Linux"
#define HAL_CPU_CLASS HAL_CPU_CLASS_1000
#define HAL_OS_POSIX_IO 1
#define HAL_OS_SOCKETS 1
#define HAL_STORAGE_SIZE            16384
#define HAL_STORAGE_SIZE_AVAILABLE  HAL_STORAGE_SIZE
#if CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_NONE
#define HAL_BOARD_LOG_DIRECTORY "logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "terrain"
#define HAL_INS_DEFAULT HAL_INS_HIL
#define HAL_BARO_DEFAULT HAL_BARO_HIL
#define HAL_COMPASS_DEFAULT HAL_COMPASS_HIL

#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_PXF || CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_ERLEBOARD
#define HAL_BOARD_LOG_DIRECTORY "/var/APM/logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "/var/APM/terrain"
#define HAL_INS_DEFAULT HAL_INS_MPU9250
#define HAL_BARO_DEFAULT HAL_BARO_MS5611_SPI
#define HAL_COMPASS_DEFAULT HAL_COMPASS_AK8963_MPU9250
#define HAL_GPIO_A_LED_PIN        61
#define HAL_GPIO_B_LED_PIN        48
#define HAL_GPIO_C_LED_PIN        117
#define HAL_GPIO_LED_ON           LOW
#define HAL_GPIO_LED_OFF          HIGH
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_BEBOP
#define HAL_BOARD_LOG_DIRECTORY "/data/ftp/internal_000/APM/logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "/data/ftp/internal_000/APM/terrain"
#define HAL_INS_DEFAULT HAL_INS_MPU60XX_I2C
#define HAL_INS_MPU60XX_I2C_BUS 2
#define HAL_INS_MPU60XX_I2C_ADDR 0x68
#define HAL_COMPASS_DEFAULT HAL_COMPASS_AK8963_I2C
#define HAL_INS_AK8963_I2C_BUS 1
#define HAL_COMPASS_AK8963_I2C_ADDR 0x0d
#define HAL_BARO_MS5607_I2C_BUS 1
#define HAL_BARO_MS5607_I2C_ADDR 0x77
#define HAL_BARO_DEFAULT HAL_BARO_MS5607
#define HAL_UTILS_HEAT HAL_LINUX_HEAT_PWM
#define HAL_LINUX_HEAT_PWM_NUM  6
#define HAL_LINUX_HEAT_KP 20000
#define HAL_LINUX_HEAT_KI 6
#define HAL_LINUX_HEAT_PERIOD_NS 125000
#define HAL_LINUX_HEAT_TARGET_TEMP 50
#define BEBOP_CAMV_PWM  9
#define BEBOP_CAMV_PWM_FREQ 43333333
#define HAL_OPTFLOW_ONBOARD_VDEV_PATH "/dev/video0"
#define HAL_OPTFLOW_ONBOARD_SUBDEV_PATH "/dev/v4l-subdev0"
#define HAL_OPTFLOW_ONBOARD_SENSOR_WIDTH 320
#define HAL_OPTFLOW_ONBOARD_SENSOR_HEIGHT 240
#define HAL_OPTFLOW_ONBOARD_OUTPUT_WIDTH 64
#define HAL_OPTFLOW_ONBOARD_OUTPUT_HEIGHT 64
#define HAL_OPTFLOW_ONBOARD_CROP_WIDTH 240
#define HAL_OPTFLOW_ONBOARD_CROP_HEIGHT 240
#define HAL_OPTFLOW_ONBOARD_NBUFS 8;
#define HAL_FLOW_PX4_MAX_FLOW_PIXEL 4
#define HAL_FLOW_PX4_BOTTOM_FLOW_FEATURE_THRESHOLD 30
#define HAL_FLOW_PX4_BOTTOM_FLOW_VALUE_THRESHOLD 5000
#define HAL_PARAM_DEFAULTS_PATH "/etc/arducopter/bebop.parm"
/* focal length 3.6 um, 2x binning in each direction
 * 240x240 crop rescaled to 64x64 */
#define HAL_FLOW_PX4_FOCAL_LENGTH_MILLIPX (2.5 / (3.6 * 2.0 * 240 / 64))
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_MINLURE
#define HAL_BOARD_LOG_DIRECTORY "/var/APM/logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "/var/APM/terrain"
#define HAL_INS_DEFAULT HAL_INS_MPU60XX_SPI
#define HAL_BARO_DEFAULT HAL_BARO_MS5611
#define HAL_BARO_MS5611_I2C_BUS 1
#define HAL_BARO_MS5611_I2C_ADDR 0x77
#define HAL_COMPASS_DEFAULT HAL_COMPASS_HMC5843_MPU6000
#define HAL_OPTFLOW_ONBOARD_VDEV_PATH "/dev/video0"
#define HAL_OPTFLOW_ONBOARD_SENSOR_WIDTH 320
#define HAL_OPTFLOW_ONBOARD_SENSOR_HEIGHT 240
#define HAL_OPTFLOW_ONBOARD_OUTPUT_WIDTH 64
#define HAL_OPTFLOW_ONBOARD_OUTPUT_HEIGHT 64
#define HAL_OPTFLOW_ONBOARD_CROP_WIDTH 240
#define HAL_OPTFLOW_ONBOARD_CROP_HEIGHT 240
#define HAL_OPTFLOW_ONBOARD_NBUFS 8;
#define HAL_FLOW_PX4_MAX_FLOW_PIXEL 4
#define HAL_FLOW_PX4_BOTTOM_FLOW_FEATURE_THRESHOLD 30
#define HAL_FLOW_PX4_BOTTOM_FLOW_VALUE_THRESHOLD 5000
#define HAL_PARAM_DEFAULTS_PATH "/etc/arducopter/minlure.parm"
/* ELP-USBFHD01M-L21
 * focal length 2.1 mm, pixel size 3 um
 * 240x240 crop rescaled to 64x64 */
#define HAL_FLOW_PX4_FOCAL_LENGTH_MILLIPX (3.0 / (2.1 * 2.0 * 240 / 64))
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_NAVIO
#define HAL_BOARD_LOG_DIRECTORY "/var/APM/logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "/var/APM/terrain"
#define HAL_INS_DEFAULT HAL_INS_MPU9250
#define HAL_BARO_DEFAULT HAL_BARO_MS5611
#define HAL_BARO_MS5611_I2C_BUS 0
#define HAL_BARO_MS5611_I2C_ADDR 0x77
#define HAL_COMPASS_DEFAULT HAL_COMPASS_AK8963_MPU9250
#define HAL_GPIO_A_LED_PIN        16
#define HAL_GPIO_B_LED_PIN        16
#define HAL_GPIO_C_LED_PIN        16
#define HAL_GPIO_LED_ON           LOW
#define HAL_GPIO_LED_OFF          HIGH
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_RASPILOT
#define HAL_BOARD_LOG_DIRECTORY "/var/APM/logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "/var/APM/terrain"
#define HAL_INS_DEFAULT HAL_INS_RASPILOT
#define HAL_BARO_DEFAULT HAL_BARO_MS5611_SPI
#define HAL_COMPASS_DEFAULT HAL_COMPASS_RASPILOT
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_ERLEBRAIN2
#define HAL_BOARD_LOG_DIRECTORY "/var/APM/logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "/var/APM/terrain"
#define HAL_INS_DEFAULT HAL_INS_MPU9250
#define HAL_BARO_DEFAULT HAL_BARO_MS5611_SPI
#define HAL_COMPASS_DEFAULT HAL_COMPASS_AK8963_MPU9250
#define HAL_GPIO_A_LED_PIN        24
#define HAL_GPIO_B_LED_PIN        25
#define HAL_GPIO_C_LED_PIN        16
#define HAL_GPIO_LED_ON           LOW
#define HAL_GPIO_LED_OFF          HIGH
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_ZYNQ
#define HAL_BOARD_LOG_DIRECTORY "/var/APM/logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "/var/APM/terrain"
// Stub the sensors out for now, at least we can build and run
#define HAL_INS_DEFAULT HAL_INS_HIL
#define HAL_BARO_DEFAULT HAL_BARO_HIL
#define HAL_COMPASS_DEFAULT HAL_COMPASS_HIL
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_BBBMINI
#define HAL_BOARD_LOG_DIRECTORY "/var/APM/logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "/var/APM/terrain"
#define HAL_INS_DEFAULT HAL_INS_MPU9250
#define HAL_BARO_DEFAULT HAL_BARO_MS5611_SPI
#define HAL_COMPASS_DEFAULT HAL_COMPASS_AK8963_MPU9250
#define HAL_GPIO_A_LED_PIN        61
#define HAL_GPIO_B_LED_PIN        48
#define HAL_GPIO_C_LED_PIN        117
#define HAL_GPIO_LED_ON           LOW
#define HAL_GPIO_LED_OFF          HIGH
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_BH
#define HAL_BOARD_LOG_DIRECTORY "/var/APM/logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "/var/APM/terrain"
#define HAL_BARO_DEFAULT HAL_BARO_MS5611
#define HAL_BARO_MS5611_I2C_BUS 0
#define HAL_BARO_MS5611_I2C_ADDR 0x77
#define HAL_INS_DEFAULT HAL_INS_BH
#define HAL_INS_MPU60XX_I2C_ADDR 0x69
#define HAL_COMPASS_DEFAULT HAL_COMPASS_BH
#define HAL_GPIO_A_LED_PIN        17
#define HAL_GPIO_B_LED_PIN        18
#define HAL_GPIO_C_LED_PIN        22
#define HAL_GPIO_LED_ON           LOW
#define HAL_GPIO_LED_OFF          HIGH
#define HAL_RCOUT_RGBLED_RED      13
#define HAL_RCOUT_RGBLED_GREEN    14
#define HAL_RCOUT_RGBLED_BLUE     15
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_QFLIGHT
#define HAL_BOARD_LOG_DIRECTORY "/var/APM/logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "/var/APM/terrain"
#define HAL_INS_DEFAULT HAL_INS_QFLIGHT
#define HAL_BARO_DEFAULT HAL_BARO_QFLIGHT
#define HAL_COMPASS_DEFAULT HAL_COMPASS_QFLIGHT
#define HAL_LINUX_UARTS_ON_TIMER_THREAD 1
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_PXFMINI
#define HAL_BOARD_LOG_DIRECTORY "/var/APM/logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "/var/APM/terrain"
#define HAL_INS_DEFAULT           HAL_INS_MPU9250
#define HAL_BARO_DEFAULT          HAL_BARO_MS5611_SPI
#define HAL_COMPASS_DEFAULT       HAL_COMPASS_AK8963_MPU9250
#define HAL_GPIO_A_LED_PIN        24
#define HAL_GPIO_B_LED_PIN        25
#define HAL_GPIO_C_LED_PIN        16
#define HAL_GPIO_LED_ON           LOW
#define HAL_GPIO_LED_OFF          HIGH  
#else
#error "no Linux board subtype set"
#endif

#ifndef HAL_LINUX_UARTS_ON_TIMER_THREAD
#define HAL_LINUX_UARTS_ON_TIMER_THREAD 0
#endif


#elif CONFIG_HAL_BOARD == HAL_BOARD_EMPTY
#define HAL_BOARD_NAME "EMPTY"
#define HAL_CPU_CLASS HAL_CPU_CLASS_16
#define HAL_STORAGE_SIZE            4096
#define HAL_STORAGE_SIZE_AVAILABLE  HAL_STORAGE_SIZE
#define HAL_INS_DEFAULT HAL_INS_HIL
#define HAL_BARO_DEFAULT HAL_BARO_HIL
#define HAL_COMPASS_DEFAULT HAL_COMPASS_HIL
#define CONFIG_HAL_BOARD_SUBTYPE HAL_BOARD_SUBTYPE_NONE

#elif CONFIG_HAL_BOARD == HAL_BOARD_QURT
#define AP_HAL_BOARD_DRIVER AP_HAL_QURT
#define HAL_BOARD_NAME "QURT"
#define HAL_CPU_CLASS HAL_CPU_CLASS_1000
#define HAL_STORAGE_SIZE            16384
#define HAL_STORAGE_SIZE_AVAILABLE  HAL_STORAGE_SIZE
#define HAL_INS_DEFAULT HAL_INS_QURT
#define HAL_BARO_DEFAULT HAL_BARO_QURT
#define HAL_COMPASS_DEFAULT HAL_COMPASS_QURT
#define CONFIG_HAL_BOARD_SUBTYPE HAL_BOARD_SUBTYPE_NONE
#define HAL_BOARD_LOG_DIRECTORY "/dev/fs/logs"
#define HAL_OS_POSIX_IO 1
#define HAL_SERIAL0_BAUD_DEFAULT 115200

#elif CONFIG_HAL_BOARD == HAL_BOARD_VRBRAIN
#define HAL_BOARD_NAME "VRBRAIN"
#define HAL_CPU_CLASS HAL_CPU_CLASS_150
#define HAL_OS_POSIX_IO 1
#define HAL_BOARD_LOG_DIRECTORY "/fs/microsd/APM/LOGS"
#define HAL_BOARD_TERRAIN_DIRECTORY "/fs/microsd/APM/TERRAIN"
#define HAL_INS_DEFAULT HAL_INS_VRBRAIN
#define HAL_BARO_DEFAULT HAL_BARO_VRBRAIN
#define HAL_COMPASS_DEFAULT HAL_COMPASS_VRBRAIN
#define HAL_SERIAL0_BAUD_DEFAULT 115200
#if  defined(CONFIG_ARCH_BOARD_VRBRAIN_V45)
#define CONFIG_HAL_BOARD_SUBTYPE HAL_BOARD_SUBTYPE_VRBRAIN_V45
#define HAL_STORAGE_SIZE            8192
#elif defined(CONFIG_ARCH_BOARD_VRBRAIN_V51)
#define CONFIG_HAL_BOARD_SUBTYPE HAL_BOARD_SUBTYPE_VRBRAIN_V51
#define HAL_STORAGE_SIZE            16384
#elif defined(CONFIG_ARCH_BOARD_VRBRAIN_V52)
#define CONFIG_HAL_BOARD_SUBTYPE HAL_BOARD_SUBTYPE_VRBRAIN_V52
#define HAL_STORAGE_SIZE            16384
#elif defined(CONFIG_ARCH_BOARD_VRUBRAIN_V51)
#define CONFIG_HAL_BOARD_SUBTYPE HAL_BOARD_SUBTYPE_VRUBRAIN_V51
#define HAL_STORAGE_SIZE            8192
#elif defined(CONFIG_ARCH_BOARD_VRUBRAIN_V52)
#define CONFIG_HAL_BOARD_SUBTYPE HAL_BOARD_SUBTYPE_VRUBRAIN_V52
#define HAL_STORAGE_SIZE            8192
#else
#define CONFIG_HAL_BOARD_SUBTYPE HAL_BOARD_SUBTYPE_NONE
#define HAL_STORAGE_SIZE            8192
#endif
#define HAL_GPIO_A_LED_PIN        25
#define HAL_GPIO_B_LED_PIN        26
#define HAL_GPIO_C_LED_PIN        27
#define HAL_GPIO_LED_ON           HIGH
#define HAL_GPIO_LED_OFF          LOW
#define EXTERNAL_LED_GPS     28    // GPS LED - AN10
#define EXTERNAL_LED_ARMED   29    // Armed LED - AN11
#define EXTERNAL_LED_MOTOR1  30    // Motor1 LED - AN8
#define EXTERNAL_LED_MOTOR2  31    // Motor2 LED - AN12

#else
#error "Unknown CONFIG_HAL_BOARD type"
#endif

#ifndef CONFIG_HAL_BOARD_SUBTYPE
#error "No CONFIG_HAL_BOARD_SUBTYPE set"
#endif

#ifndef HAL_OS_POSIX_IO
#define HAL_OS_POSIX_IO 0
#endif

#ifndef HAL_PARAM_DEFAULTS_PATH
#define HAL_PARAM_DEFAULTS_PATH NULL
#endif

#endif // __AP_HAL_BOARDS_H__
