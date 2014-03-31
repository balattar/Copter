#
# Makefile for the vrbrain-v4_APM configuration
#

#
# Use the configuration's ROMFS.
#
ROMFS_ROOT	 = $(SKETCHBOOK)/mk/VRBRAIN/ROMFS

MODULES		+= $(APM_MODULE_DIR)

#
# Board support modules
#
MODULES		+= drivers/device
MODULES		+= drivers/stm32
MODULES		+= drivers/stm32/adc
MODULES		+= drivers/stm32/tone_alarm
MODULES		+= drivers/led

MODULES		+= drivers/px4fmu
MODULES		+= drivers/boards/vrbrain-v4



MODULES		+= drivers/mpu6000
MODULES		+= drivers/hmc5883
MODULES		+= drivers/ms5611














#
# System commands
#
MODULES		+= systemcmds/mtd











MODULES		+= systemcmds/tests

MODULES		+= systemcmds/nshterm

#
# General system control
#






#
# Estimation modules (EKF/ SO3 / other filters)
#






#
# Vehicle Control
#








#
# Logging
#


#
# Unit tests
#



#
# Library modules
#
MODULES		+= modules/systemlib


MODULES		+= modules/uORB

#
# Libraries
#


MODULES		+= lib/mathlib/math/filter




























#
# Transitional support - add commands from the NuttX export archive.
#
# In general, these should move to modules over time.
#
# Each entry here is <command>.<priority>.<stacksize>.<entrypoint> but we use a helper macro
# to make the table a bit more readable.
#
define _B
	$(strip $1).$(or $(strip $2),SCHED_PRIORITY_DEFAULT).$(or $(strip $3),CONFIG_PTHREAD_STACK_DEFAULT).$(strip $4)
endef

#                  command                 priority                   stack  entrypoint
BUILTIN_COMMANDS := \
	$(call _B,  sercon,                 ,                          2048,   sercon_main                ) \
	$(call _B,  serdis,                 ,                          2048,   serdis_main                ) \
	$(call _B,  msconn,                 ,                          2048,   msconn_main                ) \
	$(call _B,   msdis,                 ,                          2048,    msdis_main                ) \
	$(call _B, sysinfo,                 ,                          2048,  sysinfo_main                )
