#pragma once

#include <hwdef.h>

#define HAL_BOARD_NAME "ChibiOS"
#define HAL_CPU_CLASS HAL_CPU_CLASS_150

#define HAL_GPIO_A_LED_PIN        0
#define HAL_GPIO_B_LED_PIN        0
#define HAL_GPIO_C_LED_PIN        0
#define HAL_GPIO_LED_ON           LOW
#define HAL_GPIO_LED_OFF          HIGH

#define HAL_HAVE_BOARD_VOLTAGE 0
#define HAL_HAVE_SAFETY_SWITCH 0

#if CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_CHIBIOS_SKYVIPER_F412
#define HAL_CHIBIOS_ARCH_F412 1

#define HAL_INS_DEFAULT HAL_INS_MPU60XX_I2C
#define HAL_INS_DEFAULT_ROTATION ROTATION_NONE
#define HAL_INS_MPU60x0_I2C_BUS 1
#define HAL_INS_MPU60x0_I2C_ADDR 0x68

#define HAL_GPIO_RADIO_RESET       1
#define HAL_GPIO_RADIO_IRQ         2

#define HAL_RCINPUT_WITH_AP_RADIO 1
#define STORAGE_FLASH_PAGE		1
#define HAL_STORAGE_SIZE        8192
#define HAL_USE_EKF3

#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_CHIBIOS_FMUV3
#define HAL_CHIBIOS_ARCH_FMUV3 1

#define HAL_BARO_MS5611_NAME "ms5611"
#define HAL_BARO_MS5611_SPI_INT_NAME "ms5611_int"
#define HAL_BARO_MS5611_SPI_EXT_NAME "ms5611_ext"

#define HAL_INS_MPU60x0_NAME "mpu6000"
#define HAL_INS_MPU60x0_EXT_NAME "mpu6000_ext"

#define HAL_INS_LSM9DS0_G_NAME "lsm9ds0_g"
#define HAL_INS_LSM9DS0_A_NAME "lsm9ds0_am"

#define HAL_INS_LSM9DS0_EXT_G_NAME "lsm9ds0_ext_g"
#define HAL_INS_LSM9DS0_EXT_A_NAME "lsm9ds0_ext_am"

#define HAL_INS_MPU9250_NAME "mpu9250"
#define HAL_INS_MPU9250_EXT_NAME "mpu9250_ext"

#define HAL_INS_MPU6500_NAME "mpu6500"

#define HAL_INS_ICM20608_NAME "icm20608"
#define HAL_INS_ICM20608_AM_NAME "icm20608-am"
#define HAL_INS_ICM20608_EXT_NAME "icm20608_ext"

#define HAL_COMPASS_HMC5843_NAME "hmc5843"
#define HAL_COMPASS_LIS3MDL_NAME "lis3mdl"

#define HAL_BOARD_LOG_DIRECTORY "/APM/LOGS"
#define HAL_BOARD_TERRAIN_DIRECTORY "/APM/TERRAIN"

#define STORAGE_FLASH_PAGE		22
#define HAL_STORAGE_SIZE        16384

// enable RAMTROM parameter storage
#define HAL_WITH_RAMTRON 1

// pixhawk2 cube has an IMU header
#define HAL_HAVE_IMU_HEATER         1

// enable FAT filesystem
#define HAL_OS_FATFS_IO 1

#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_CHIBIOS_FMUV4
#define HAL_CHIBIOS_ARCH_FMUV4 1

#define HAL_BOARD_LOG_DIRECTORY "/APM/LOGS"

#define STORAGE_FLASH_PAGE		22
#define HAL_STORAGE_SIZE        16384

// enable RAMTROM parameter storage
#define HAL_WITH_RAMTRON 1

// enable FAT filesystem
#define HAL_OS_FATFS_IO 1

#elif CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_CHIBIOS_MINDPXV2
#define HAL_CHIBIOS_ARCH_MINDPXV2 1

#define HAL_BOARD_LOG_DIRECTORY "/APM/LOGS"

#define STORAGE_FLASH_PAGE		22
#define HAL_STORAGE_SIZE        16384

// enable RAMTROM parameter storage
#define HAL_WITH_RAMTRON 1

// enable FAT filesystem
#define HAL_OS_FATFS_IO 1

#endif // CONFIG_HAL_BOARD_SUBTYPE

#define HAL_STORAGE_SIZE_AVAILABLE  HAL_STORAGE_SIZE

#ifndef HAL_WITH_IO_MCU
#define HAL_WITH_IO_MCU 0
#endif

#ifndef HAL_WITH_RAMTRON
#define HAL_WITH_RAMTRON 0
#endif

/* string names for well known SPI devices */
#define HAL_BARO_MS5611_NAME "ms5611"
#define HAL_BARO_MS5611_SPI_INT_NAME "ms5611_int"
#define HAL_BARO_MS5611_SPI_EXT_NAME "ms5611_ext"

#define HAL_INS_MPU60x0_NAME "mpu6000"
#define HAL_INS_MPU60x0_EXT_NAME "mpu6000_ext"

#define HAL_INS_LSM9DS0_G_NAME "lsm9ds0_g"
#define HAL_INS_LSM9DS0_A_NAME "lsm9ds0_am"

#define HAL_INS_LSM9DS0_EXT_G_NAME "lsm9ds0_ext_g"
#define HAL_INS_LSM9DS0_EXT_A_NAME "lsm9ds0_ext_am"

#define HAL_INS_MPU9250_NAME "mpu9250"
#define HAL_INS_MPU9250_EXT_NAME "mpu9250_ext"

#define HAL_INS_MPU6500_NAME "mpu6500"

#define HAL_INS_ICM20608_NAME "icm20608"
#define HAL_INS_ICM20608_AM_NAME "icm20608-am"
#define HAL_INS_ICM20608_EXT_NAME "icm20608_ext"

#define HAL_COMPASS_HMC5843_NAME "hmc5843"
#define HAL_COMPASS_LIS3MDL_NAME "lis3mdl"
