# PX4 build is via external build system

ifneq ($(PX4_ROOT),)
$(error PX4_ROOT found in config.mk - Please see http://dev.ardupilot.com/wiki/git-submodules/)
endif

ifneq ($(NUTTX_SRC),)
$(error NUTTX_SRC found in config.mk - Please see http://dev.ardupilot.com/wiki/git-submodules/)
endif

ifneq ($(UAVCAN_DIR),)
$(error UAVCAN_DIR found in config.mk - Please see http://dev.ardupilot.com/wiki/git-submodules/)
endif

# these can be overridden in developer.mk
PX4FIRMWARE_DIRECTORY ?= $(SKETCHBOOK)/modules/PX4Firmware
PX4NUTTX_DIRECTORY ?= $(SKETCHBOOK)/modules/PX4Firmware
UAVCAN_DIRECTORY ?= $(SKETCHBOOK)/modules/uavcan

PX4_ROOT := $(shell cd $(PX4FIRMWARE_DIRECTORY) && pwd)
NUTTX_ROOT := $(shell cd $(PX4NUTTX_DIRECTORY) && pwd)
NUTTX_SRC := $(NUTTX_ROOT)/nuttx/
UAVCAN_DIR=$(shell cd $(UAVCAN_DIRECTORY) && pwd)/

# warn if user has old PX4Firmware or PX4NuttX trees
ifneq ($(wildcard $(SKETCHBOOK)/../PX4Firmware),)
$(warning *** You have an old PX4Firmware tree - see http://dev.ardupilot.com/wiki/git-submodules/)
endif
ifneq ($(wildcard $(SKETCHBOOK)/../PX4NuttX),)
$(warning *** You have an old PX4NuttX tree - see http://dev.ardupilot.com/wiki/git-submodules/)
endif
ifneq ($(wildcard $(SKETCHBOOK)/../uavcan),)
$(warning *** You have an old uavcan tree - see http://dev.ardupilot.com/wiki/git-submodules/)
endif

NUTTX_GIT_VERSION := $(shell cd $(NUTTX_SRC) && git rev-parse HEAD | cut -c1-8)
PX4_GIT_VERSION   := $(shell cd $(PX4_ROOT) && git rev-parse HEAD | cut -c1-8)

EXTRAFLAGS += -DNUTTX_GIT_VERSION="\"$(NUTTX_GIT_VERSION)\""
EXTRAFLAGS += -DPX4_GIT_VERSION="\"$(PX4_GIT_VERSION)\""
EXTRAFLAGS += -DUAVCAN=1

# we have different config files for V1 and V2
PX4_V1_CONFIG_FILE=$(MK_DIR)/PX4/config_px4fmu-v1_APM.mk
PX4_V2_CONFIG_FILE=$(MK_DIR)/PX4/config_px4fmu-v2_APM.mk

SKETCHFLAGS=$(SKETCHLIBINCLUDES) -I$(PWD) -DARDUPILOT_BUILD -DTESTS_MATHLIB_DISABLE -DCONFIG_HAL_BOARD=HAL_BOARD_PX4 -DSKETCHNAME="\\\"$(SKETCH)\\\"" -DSKETCH_MAIN=ArduPilot_main -DAPM_BUILD_DIRECTORY=APM_BUILD_$(SKETCH)

WARNFLAGS = -Werror -Wno-psabi -Wno-packed -Wno-error=double-promotion -Wno-error=unused-variable -Wno-error=reorder -Wno-error=float-equal -Wno-error=pmf-conversions -Wno-error=missing-declarations -Wno-error=unused-function

# avoid PX4 submodules
export GIT_SUBMODULES_ARE_EVIL = 1

PYTHONPATH=$(SKETCHBOOK)/mk/PX4/Tools/genmsg/src:$(SKETCHBOOK)/mk/PX4/Tools/gencpp/src
export PYTHONPATH

PX4_MAKE = $(v) GIT_SUBMODULES_ARE_EVIL=1 ARDUPILOT_BUILD=1 make -C $(SKETCHBOOK) -f $(PX4_ROOT)/Makefile EXTRADEFINES="$(SKETCHFLAGS) $(WARNFLAGS) "'$(EXTRAFLAGS)' APM_MODULE_DIR=$(SKETCHBOOK) SKETCHBOOK=$(SKETCHBOOK) CCACHE=$(CCACHE) PX4_ROOT=$(PX4_ROOT) NUTTX_SRC=$(NUTTX_SRC) MAXOPTIMIZATION="-Os" UAVCAN_DIR=$(UAVCAN_DIR)
PX4_MAKE_ARCHIVES = make -C $(PX4_ROOT) NUTTX_SRC=$(NUTTX_SRC) CCACHE=$(CCACHE) archives MAXOPTIMIZATION="-Os"

HASHADDER_FLAGS += --ardupilot "$(SKETCHBOOK)"

ifneq ($(wildcard $(PX4_ROOT)),)
HASHADDER_FLAGS += --px4 "$(PX4_ROOT)"
endif
ifneq ($(wildcard $(NUTTX_SRC)/..),)
HASHADDER_FLAGS += --nuttx "$(NUTTX_SRC)/.."
endif
HASHADDER_FLAGS += --uavcan "$(UAVCAN_DIR)"

.PHONY: module_mk
module_mk:
	$(v) echo "Building $(SKETCHBOOK)/module.mk"
	$(RULEHDR)
	$(v) echo "# Auto-generated file - do not edit" > $(SKETCHBOOK)/module.mk.new
	$(v) echo "MODULE_COMMAND = ArduPilot" >> $(SKETCHBOOK)/module.mk.new
	$(v) echo "SRCS = $(wildcard $(SRCROOT)/*.cpp) $(SKETCHLIBSRCSRELATIVE)" >> $(SKETCHBOOK)/module.mk.new
	$(v) echo "MODULE_STACKSIZE = 4096" >> $(SKETCHBOOK)/module.mk.new
	$(v) echo "EXTRACXXFLAGS = -Wframe-larger-than=1200" >> $(SKETCHBOOK)/module.mk.new
	$(v) cmp $(SKETCHBOOK)/module.mk $(SKETCHBOOK)/module.mk.new 2>/dev/null || mv $(SKETCHBOOK)/module.mk.new $(SKETCHBOOK)/module.mk
	$(v) rm -f $(SKETCHBOOK)/module.mk.new

px4-v1: $(BUILDROOT)/make.flags CHECK_MODULES $(PX4_ROOT)/Archives/px4fmu-v1.export $(SKETCHCPP) module_mk px4-io-v1
	$(RULEHDR)
	$(v) rm -f $(PX4_ROOT)/makefiles/$(PX4_V1_CONFIG_FILE)
	$(v) cp $(PX4_V1_CONFIG_FILE) $(PX4_ROOT)/makefiles/nuttx/
	$(v) $(PX4_MAKE) px4fmu-v1_APM
	$(v) /bin/rm -f $(SKETCH)-v1.px4
	$(v) cp $(PX4_ROOT)/Images/px4fmu-v1_APM.px4 $(SKETCH)-v1.px4
	$(v) $(SKETCHBOOK)/Tools/scripts/add_git_hashes.py $(HASHADDER_FLAGS) "$(SKETCH)-v1.px4" "$(SKETCH)-v1.px4"
	$(v) echo "PX4 $(SKETCH) Firmware is in $(SKETCH)-v1.px4"

px4-v2: $(BUILDROOT)/make.flags CHECK_MODULES $(PX4_ROOT)/Archives/px4fmu-v2.export $(SKETCHCPP) module_mk px4-io-v2
	$(RULEHDR)
	$(v) rm -f $(PX4_ROOT)/makefiles/$(PX4_V2_CONFIG_FILE)
	$(v) cp $(PX4_V2_CONFIG_FILE) $(PX4_ROOT)/makefiles/nuttx/
	$(PX4_MAKE) px4fmu-v2_APM
	$(v) /bin/rm -f $(SKETCH)-v2.px4
	$(v) cp $(PX4_ROOT)/Images/px4fmu-v2_APM.px4 $(SKETCH)-v2.px4
	$(v) $(SKETCHBOOK)/Tools/scripts/add_git_hashes.py $(HASHADDER_FLAGS) "$(SKETCH)-v2.px4" "$(SKETCH)-v2.px4"
	$(v) echo "PX4 $(SKETCH) Firmware is in $(SKETCH)-v2.px4"

px4: px4-v1 px4-v2

px4-clean: clean CHECK_MODULES px4-archives-clean px4-cleandep
	$(v) /bin/rm -rf $(PX4_ROOT)/makefiles/build $(PX4_ROOT)/Build

px4-cleandep: clean
	$(v) find $(PX4_ROOT)/Build -type f -name '*.d' | xargs rm -f
	$(v) find $(SKETCHBOOK)/$(SKETCH) -type f -name '*.d' | xargs rm -f

px4-v1-upload: px4-v1
	$(RULEHDR)
	$(v) $(PX4_MAKE) px4fmu-v1_APM upload

px4-v2-upload: px4-v2
	$(RULEHDR)
	$(v) $(PX4_MAKE) px4fmu-v2_APM upload

px4-upload: px4-v1-upload

px4-archives-clean:
	$(v) /bin/rm -rf $(PX4_ROOT)/Archives

px4-io-v1: $(PX4_ROOT)/Archives/px4io-v1.export
	$(v) make -C $(PX4_ROOT) px4io-v1_default
	$(v) /bin/rm -f px4io-v1.bin
	$(v) cp $(PX4_ROOT)/Images/px4io-v1_default.bin px4io-v1.bin
	$(v) cp $(PX4_ROOT)/Build/px4io-v1_default.build/firmware.elf px4io-v1.elf
	$(v) mkdir -p $(MK_DIR)/PX4/ROMFS/px4io/
	$(v) rm -f $(MK_DIR)/PX4/ROMFS/px4io/px4io.bin
	$(v) cp px4io-v1.bin $(MK_DIR)/PX4/ROMFS/px4io/px4io.bin
	$(v) mkdir -p $(MK_DIR)/PX4/ROMFS/bootloader/
	$(v) rm -f $(MK_DIR)/PX4/ROMFS/bootloader/fmu_bl.bin
	$(v) cp $(SKETCHBOOK)/mk/PX4/bootloader/px4fmu_bl.bin $(MK_DIR)/PX4/ROMFS/bootloader/fmu_bl.bin
	$(v) echo "PX4IOv1 Firmware is in px4io-v1.bin"


px4-io-v2: $(PX4_ROOT)/Archives/px4io-v2.export
	$(v) make -C $(PX4_ROOT) px4io-v2_default
	$(v) /bin/rm -f px4io-v2.bin
	$(v) cp $(PX4_ROOT)/Build/px4io-v2_default.build/firmware.bin px4io-v2.bin
	$(v) cp $(PX4_ROOT)/Images/px4io-v2_default.bin px4io-v2.bin
	$(v) cp $(PX4_ROOT)/Build/px4io-v2_default.build/firmware.elf px4io-v2.elf
	$(v) mkdir -p $(MK_DIR)/PX4/ROMFS/px4io/
	$(v) rm -f $(MK_DIR)/PX4/ROMFS/px4io/px4io.bin
	$(v) cp px4io-v2.bin $(MK_DIR)/PX4/ROMFS/px4io/px4io.bin
	$(v) mkdir -p $(MK_DIR)/PX4/ROMFS/bootloader/
	$(v) rm -f $(MK_DIR)/PX4/ROMFS/bootloader/fmu_bl.bin
	$(v) cp $(SKETCHBOOK)/mk/PX4/bootloader/px4fmuv2_bl.bin $(MK_DIR)/PX4/ROMFS/bootloader/fmu_bl.bin
	$(v) echo "PX4IOv2 Firmware is in px4io-v2.bin"

px4-io: px4-io-v1 px4-io-v2


$(PX4_ROOT)/Archives/px4fmu-v1.export:
	$(v) $(PX4_MAKE_ARCHIVES) BOARDS="px4fmu-v1"

$(PX4_ROOT)/Archives/px4fmu-v2.export:
	$(v) $(PX4_MAKE_ARCHIVES) BOARDS="px4fmu-v2"

$(PX4_ROOT)/Archives/px4io-v1.export:
	$(v) $(PX4_MAKE_ARCHIVES) BOARDS="px4io-v1"

$(PX4_ROOT)/Archives/px4io-v2.export:
	$(v) $(PX4_MAKE_ARCHIVES) BOARDS="px4io-v2"

px4-archives:
	$(v) $(PX4_MAKE_ARCHIVES) BOARDS="px4io-v1 px4io-v2 px4fmu-v1 px4fmu-v2"
