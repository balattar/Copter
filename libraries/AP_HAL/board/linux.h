#pragma once

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
#define HAL_INS_DEFAULT HAL_INS_MPU9250_SPI
#define HAL_INS_MPU9250_NAME "mpu9250"
#define HAL_BARO_DEFAULT HAL_BARO_MS5611_SPI
#define HAL_BARO_MS5611_NAME "ms5611"
#define HAL_COMPASS_DEFAULT HAL_COMPASS_AK8963_MPU9250
#define HAL_COMPASS_HMC5843_I2C_BUS 1
#define HAL_COMPASS_HMC5843_I2C_ADDR 0x1E
#define HAL_GPIO_A_LED_PIN        61
#define HAL_GPIO_B_LED_PIN        48
#define HAL_GPIO_C_LED_PIN        117
#define HAL_GPIO_LED_ON           LOW
#define HAL_GPIO_LED_OFF          HIGH
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_BEBOP
#define HAL_BOARD_LOG_DIRECTORY "/data/ftp/internal_000/APM/logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "/data/ftp/internal_000/APM/terrain"
#define HAL_INS_DEFAULT HAL_INS_MPU60XX_I2C
#define HAL_INS_MPU60x0_I2C_BUS 2
#define HAL_INS_MPU60x0_I2C_ADDR 0x68
#define HAL_COMPASS_DEFAULT HAL_COMPASS_AK8963_I2C
#define HAL_COMPASS_AK8963_I2C_BUS 1
#define HAL_COMPASS_AK8963_I2C_ADDR 0x0d
#define HAL_BARO_DEFAULT HAL_BARO_MS5607_I2C
#define HAL_BARO_MS5607_I2C_BUS 1
#define HAL_BARO_MS5607_I2C_ADDR 0x77
#define HAL_HAVE_IMU_HEATER 1
#define HAL_IMU_TEMP_DEFAULT 55
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
#define HAL_OPTFLOW_ONBOARD_NBUFS 8
#define HAL_FLOW_PX4_MAX_FLOW_PIXEL 4
#define HAL_FLOW_PX4_BOTTOM_FLOW_FEATURE_THRESHOLD 30
#define HAL_FLOW_PX4_BOTTOM_FLOW_VALUE_THRESHOLD 5000
#define HAL_PARAM_DEFAULTS_PATH "/etc/arducopter/bebop.parm"
#define HAL_RCOUT_BEBOP_BLDC_I2C_BUS 1
#define HAL_RCOUT_BEBOP_BLDC_I2C_ADDR 0x08
/* focal length 3.6 um, 2x binning in each direction
 * 240x240 crop rescaled to 64x64 */
#define HAL_RANGEFINDER_LIGHTWARE_I2C_BUS 0
#define HAL_FLOW_PX4_FOCAL_LENGTH_MILLIPX (2.5 / (3.6 * 2.0 * 240 / 64))
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_DISCO
#define HAL_BOARD_LOG_DIRECTORY "/data/ftp/internal_000/APM/logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "/data/ftp/internal_000/APM/terrain"
#define HAL_INS_DEFAULT HAL_INS_MPU60XX_I2C
#define HAL_INS_MPU60x0_I2C_BUS 2
#define HAL_INS_MPU60x0_I2C_ADDR 0x68
#define HAL_COMPASS_DEFAULT HAL_COMPASS_AK8963_I2C
#define HAL_COMPASS_AK8963_I2C_BUS 1
#define HAL_COMPASS_AK8963_I2C_ADDR 0x0d
#define HAL_BARO_DEFAULT HAL_BARO_MS5607_I2C
#define HAL_BARO_MS5607_I2C_BUS 1
#define HAL_BARO_MS5607_I2C_ADDR 0x77
#define HAL_AIRSPEED_MS4515DO_I2C_BUS 1
#define HAL_UTILS_HEAT HAL_LINUX_HEAT_PWM
#define HAL_LINUX_HEAT_PWM_NUM  10
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
#define HAL_OPTFLOW_ONBOARD_NBUFS 8
#define HAL_FLOW_PX4_MAX_FLOW_PIXEL 4
#define HAL_FLOW_PX4_BOTTOM_FLOW_FEATURE_THRESHOLD 30
#define HAL_FLOW_PX4_BOTTOM_FLOW_VALUE_THRESHOLD 5000
#define HAL_RCOUT_DISCO_BLDC_I2C_BUS 1
#define HAL_RCOUT_DISCO_BLDC_I2C_ADDR 0x08
#define HAL_PARAM_DEFAULTS_PATH "/etc/arduplane/disco.parm"
/* focal length 3.6 um, 2x binning in each direction
 * 240x240 crop rescaled to 64x64 */
#define HAL_FLOW_PX4_FOCAL_LENGTH_MILLIPX (2.5 / (3.6 * 2.0 * 240 / 64))
#define HAL_RANGEFINDER_LIGHTWARE_I2C_BUS 0
// the disco has challenges with its magnetic setup
#define AP_ARMING_COMPASS_OFFSETS_MAX 2200
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_MINLURE
#define HAL_BOARD_LOG_DIRECTORY "/var/APM/logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "/var/APM/terrain"
#define HAL_INS_DEFAULT HAL_INS_MPU60XX_SPI
#define HAL_INS_MPU60x0_NAME "mpu6000"
#define HAL_BARO_DEFAULT HAL_BARO_MS5611_I2C
#define HAL_BARO_MS5611_I2C_BUS 10
#define HAL_BARO_MS5611_I2C_ADDR 0x77
/* Internal and external compasses */
#define HAL_COMPASS_HMC5843_I2C_ADDR 0x1E
#define HAL_OPTFLOW_ONBOARD_VDEV_PATH "/dev/video0"
#define HAL_OPTFLOW_ONBOARD_SENSOR_WIDTH 320
#define HAL_OPTFLOW_ONBOARD_SENSOR_HEIGHT 240
#define HAL_OPTFLOW_ONBOARD_OUTPUT_WIDTH 64
#define HAL_OPTFLOW_ONBOARD_OUTPUT_HEIGHT 64
#define HAL_OPTFLOW_ONBOARD_CROP_WIDTH 240
#define HAL_OPTFLOW_ONBOARD_CROP_HEIGHT 240
#define HAL_OPTFLOW_ONBOARD_NBUFS 8
#define HAL_FLOW_PX4_MAX_FLOW_PIXEL 4
#define HAL_FLOW_PX4_BOTTOM_FLOW_FEATURE_THRESHOLD 30
#define HAL_FLOW_PX4_BOTTOM_FLOW_VALUE_THRESHOLD 5000
/* ELP-USBFHD01M-L21
 * focal length 2.1 mm, pixel size 3 um
 * 240x240 crop rescaled to 64x64 */
#define HAL_FLOW_PX4_FOCAL_LENGTH_MILLIPX (3.0 / (2.1 * 2.0 * 240 / 64))
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_NAVIO
#define HAL_BOARD_LOG_DIRECTORY "/var/APM/logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "/var/APM/terrain"
#define HAL_INS_DEFAULT HAL_INS_MPU9250_SPI
#define HAL_INS_MPU9250_NAME "mpu9250"
#define HAL_BARO_DEFAULT HAL_BARO_MS5611_I2C
#define HAL_BARO_MS5611_I2C_BUS 1
#define HAL_BARO_MS5611_I2C_ADDR 0x77
#define HAL_COMPASS_DEFAULT HAL_COMPASS_NAVIO
/* HMC5843 can be an external compass */
#define HAL_COMPASS_HMC5843_I2C_BUS 1
#define HAL_COMPASS_HMC5843_I2C_ADDR 0x1E
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_NAVIO2
#define HAL_BOARD_LOG_DIRECTORY "/var/APM/logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "/var/APM/terrain"
#define HAL_INS_DEFAULT HAL_INS_MPU9250_SPI
#define HAL_INS_MPU9250_NAME "mpu9250"
#define HAL_BARO_DEFAULT HAL_BARO_MS5611_I2C
#define HAL_BARO_MS5611_I2C_BUS 1
#define HAL_BARO_MS5611_I2C_ADDR 0x77
#define HAL_COMPASS_DEFAULT HAL_COMPASS_NAVIO2
/* HMC5843 can be an external compass */
#define HAL_COMPASS_HMC5843_I2C_BUS 1
#define HAL_COMPASS_HMC5843_I2C_ADDR 0x1E
#define HAL_GPIO_A_LED_PIN        16
#define HAL_GPIO_B_LED_PIN        16
#define HAL_GPIO_C_LED_PIN        16
#define HAL_GPIO_LED_ON           LOW
#define HAL_GPIO_LED_OFF          HIGH
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_RASPILOT
#define HAL_BOARD_LOG_DIRECTORY "/var/APM/logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "/var/APM/terrain"
#define HAL_INS_DEFAULT HAL_INS_RASPILOT
#define HAL_INS_MPU60x0_NAME "mpu6000"
#define HAL_INS_LSM9DS0_G_NAME "lsm9ds0_g"
#define HAL_INS_LSM9DS0_A_NAME "lsm9ds0_am"
#define HAL_BARO_DEFAULT HAL_BARO_MS5611_SPI
#define HAL_BARO_MS5611_NAME "ms5611"
#define HAL_COMPASS_DEFAULT HAL_COMPASS_RASPILOT
/* HMC5843 can be an external compass */
#define HAL_COMPASS_HMC5843_I2C_BUS 1
#define HAL_COMPASS_HMC5843_I2C_ADDR 0x1E
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_ERLEBRAIN2
#define HAL_BOARD_LOG_DIRECTORY "/var/APM/logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "/var/APM/terrain"
#define HAL_INS_DEFAULT HAL_INS_MPU9250_SPI
#define HAL_INS_MPU9250_NAME "mpu9250"
#define HAL_BARO_DEFAULT HAL_BARO_MS5611_SPI
#define HAL_BARO_MS5611_NAME "ms5611"
#define HAL_COMPASS_DEFAULT HAL_COMPASS_AK8963_MPU9250
/* HMC5843 can be an external compass */
#define HAL_COMPASS_HMC5843_I2C_BUS 1
#define HAL_COMPASS_HMC5843_I2C_ADDR 0x1E
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
#define HAL_INS_DEFAULT HAL_INS_BBBMINI
#define HAL_INS_MPU9250_NAME "mpu9250"
#define HAL_INS_MPU9250_NAME_EXT "mpu9250ext"
#define HAL_BARO_DEFAULT HAL_BARO_MS5611_SPI
#define HAL_BARO_MS5611_NAME "ms5611"
#define HAL_COMPASS_DEFAULT HAL_COMPASS_BBBMINI
/* HMC5843 can be an external compass */
#define HAL_COMPASS_HMC5843_I2C_BUS 2
#define HAL_COMPASS_HMC5843_I2C_ADDR 0x1E
#define HAL_OPTFLOW_ONBOARD_VDEV_PATH "/dev/video0"
#define HAL_OPTFLOW_ONBOARD_SENSOR_WIDTH 320
#define HAL_OPTFLOW_ONBOARD_SENSOR_HEIGHT 240
#define HAL_OPTFLOW_ONBOARD_OUTPUT_WIDTH 64
#define HAL_OPTFLOW_ONBOARD_OUTPUT_HEIGHT 64
#define HAL_OPTFLOW_ONBOARD_CROP_WIDTH 240
#define HAL_OPTFLOW_ONBOARD_CROP_HEIGHT 240
#define HAL_OPTFLOW_ONBOARD_NBUFS 8
#define HAL_FLOW_PX4_MAX_FLOW_PIXEL 4
#define HAL_FLOW_PX4_BOTTOM_FLOW_FEATURE_THRESHOLD 30
#define HAL_FLOW_PX4_BOTTOM_FLOW_VALUE_THRESHOLD 5000
/* ELP-USBFHD01M-L21
 * focal length 2.1 mm, pixel size 3 um
 * 240x240 crop rescaled to 64x64 */
#define HAL_FLOW_PX4_FOCAL_LENGTH_MILLIPX (3.0 / (2.1 * 2.0 * 240 / 64))
#define HAL_RANGEFINDER_LIGHTWARE_I2C_BUS 2
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_BH
#define HAL_BOARD_LOG_DIRECTORY "/var/APM/logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "/var/APM/terrain"
#define HAL_BARO_DEFAULT HAL_BARO_MS5611_I2C
#define HAL_BARO_MS5611_I2C_BUS 1
#define HAL_BARO_MS5611_I2C_ADDR 0x77
#define HAL_INS_DEFAULT HAL_INS_BH
#define HAL_INS_MPU9250_NAME "mpu9250"
#define HAL_INS_MPU60x0_I2C_BUS 1
#define HAL_INS_MPU60x0_I2C_ADDR 0x69
#define HAL_COMPASS_DEFAULT HAL_COMPASS_BH
/* HMC5843 can be an external compass */
#define HAL_COMPASS_HMC5843_I2C_BUS 1
#define HAL_COMPASS_HMC5843_I2C_ADDR 0x1E
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
#define HAL_INS_DEFAULT           HAL_INS_MPU9250_SPI
#define HAL_INS_MPU9250_NAME "mpu9250"
#define HAL_BARO_DEFAULT          HAL_BARO_MS5611_SPI
#define HAL_BARO_MS5611_NAME "ms5611"
#define HAL_COMPASS_DEFAULT       HAL_COMPASS_AK8963_MPU9250
/* HMC5843 can be an external compass */
#define HAL_COMPASS_HMC5843_I2C_BUS 1
#define HAL_COMPASS_HMC5843_I2C_ADDR 0x1E
#define HAL_GPIO_A_LED_PIN        24
#define HAL_GPIO_B_LED_PIN        25
#define HAL_GPIO_C_LED_PIN        16
#define HAL_GPIO_LED_ON           LOW
#define HAL_GPIO_LED_OFF          HIGH
#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_AERO
#define HAL_BOARD_LOG_DIRECTORY "/var/APM/logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "/var/APM/terrain"
#define HAL_INS_DEFAULT HAL_INS_AERO
#define HAL_BARO_DEFAULT HAL_BARO_MS5611_I2C
#define HAL_BARO_MS5611_I2C_BUS 2
#define HAL_BARO_MS5611_I2C_ADDR 0x76
#define HAL_COMPASS_DEFAULT HAL_COMPASS_AERO
#define HAL_COMPASS_BMM150_I2C_BUS 2
#define HAL_COMPASS_BMM150_I2C_ADDR 0x12
#else
#error "no Linux board subtype set"
#endif

#ifndef HAL_COMPASS_DEFAULT
#define HAL_COMPASS_DEFAULT -1
#endif

#ifndef HAL_LINUX_UARTS_ON_TIMER_THREAD
#define HAL_LINUX_UARTS_ON_TIMER_THREAD 0
#endif

#ifndef HAL_OPTFLOW_PX4FLOW_I2C_ADDRESS
#define HAL_OPTFLOW_PX4FLOW_I2C_ADDRESS 0x42
#endif

#ifndef HAL_OPTFLOW_PX4FLOW_I2C_BUS
#define HAL_OPTFLOW_PX4FLOW_I2C_BUS 1
#endif

