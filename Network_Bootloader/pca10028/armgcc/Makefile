PROJECT_NAME     := secure_dfu_secure_dfu_ble_s130_pca20006
TARGETS          := bootloader_nrf51822_xxac_s130
OUTPUT_DIRECTORY := _build

SDK_PATH := C:/nRF5_SDK_12.3.0_d7731ad
PROJ_DIR := ../..

$(OUTPUT_DIRECTORY)/bootloader_nrf51822_xxac_s130.out: \
  LINKER_SCRIPT  := $(PROJ_DIR)/secure_dfu_gcc_nrf51.ld

# Source files common to all targets
SRC_FILES += \
  $(SDK_PATH)/components/libraries/scheduler/app_scheduler.c \
  $(SDK_PATH)/components/libraries/util/app_error_weak.c \
  $(SDK_PATH)/components/libraries/timer/app_timer.c \
  $(SDK_PATH)/components/libraries/timer/app_timer_appsh.c \
  $(SDK_PATH)/components/libraries/util/app_util_platform.c \
  $(SDK_PATH)/components/libraries/crc32/crc32.c \
  $(SDK_PATH)/components/libraries/ecc/ecc.c \
  $(SDK_PATH)/components/libraries/fstorage/fstorage.c \
  $(SDK_PATH)/components/libraries/hci/hci_mem_pool.c \
  $(SDK_PATH)/components/libraries/util/nrf_assert.c \
  $(SDK_PATH)/components/libraries/crypto/nrf_crypto.c \
  $(SDK_PATH)/components/libraries/queue/nrf_queue.c \
  $(SDK_PATH)/components/libraries/util/sdk_errors.c \
  $(SDK_PATH)/components/libraries/sha256/sha256.c \
  $(SDK_PATH)/components/boards/boards.c \
  $(SDK_PATH)/components/drivers_nrf/common/nrf_drv_common.c \
  $(SDK_PATH)/components/drivers_nrf/rng/nrf_drv_rng.c \
  $(SDK_PATH)/components/drivers_nrf/hal/nrf_nvmc.c \
  $(SDK_PATH)/components/libraries/bootloader/ble_dfu/nrf_ble_dfu.c \
  $(PROJ_DIR)/dfu-cc.pb.c \
  $(PROJ_DIR)/dsa_public_key.c \
  $(PROJ_DIR)/dfu_req_handling.c \
  $(PROJ_DIR)/main.c \
  $(PROJ_DIR)/nrf_dfu.c \
  $(SDK_PATH)/components/ble/common/ble_advdata.c \
  $(SDK_PATH)/components/ble/common/ble_conn_params.c \
  $(SDK_PATH)/components/ble/common/ble_srv_common.c \
  $(SDK_PATH)/external/nano-pb/pb_common.c \
  $(SDK_PATH)/external/nano-pb/pb_decode.c \
  $(SDK_PATH)/components/toolchain/gcc/gcc_startup_nrf51.S \
  $(SDK_PATH)/components/toolchain/system_nrf51.c \
  $(SDK_PATH)/components/softdevice/common/softdevice_handler/softdevice_handler.c \
  $(SDK_PATH)/components/softdevice/common/softdevice_handler/softdevice_handler_appsh.c \
  $(SDK_PATH)/components/libraries/bootloader/nrf_bootloader.c \
  $(SDK_PATH)/components/libraries/bootloader/nrf_bootloader_app_start.c \
  $(SDK_PATH)/components/libraries/bootloader/nrf_bootloader_info.c \
  $(SDK_PATH)/components/libraries/bootloader/dfu/nrf_dfu_flash.c \
  $(SDK_PATH)/components/libraries/bootloader/dfu/nrf_dfu_mbr.c \
  $(SDK_PATH)/components/libraries/bootloader/dfu/nrf_dfu_settings.c \
  $(SDK_PATH)/components/libraries/bootloader/dfu/nrf_dfu_transport.c \
  $(SDK_PATH)/components/libraries/bootloader/dfu/nrf_dfu_utils.c \

# Include folders common to all targets
INC_FOLDERS += \
  $(SDK_PATH)/components/drivers_nrf/rng \
  $(SDK_PATH)/components/device \
  $(SDK_PATH)/components/drivers_nrf/hal \
  $(SDK_PATH)/components/libraries/sha256 \
  $(SDK_PATH)/components/libraries/crc32 \
  $(SDK_PATH)/components/libraries/experimental_section_vars \
  $(PROJ_DIR) \
  $(SDK_PATH)/components/libraries/fstorage \
  $(SDK_PATH)/components/libraries/util \
  $(SDK_PATH)/components \
  $(SDK_PATH)/components/softdevice/common/softdevice_handler \
  $(SDK_PATH)/components/libraries/timer \
  $(SDK_PATH)/components/drivers_nrf/clock \
  $(SDK_PATH)/components/softdevice/s130/headers \
  ../config \
  $(SDK_PATH)/components/libraries/log/src \
  $(SDK_PATH)/components/libraries/bootloader/dfu \
  $(SDK_PATH)/components/drivers_nrf/delay \
  $(SDK_PATH)/components/ble/common \
  $(SDK_PATH)/components/drivers_nrf/common \
  $(SDK_PATH)/components/libraries/svc \
  $(SDK_PATH)/components/libraries/scheduler \
  $(SDK_PATH)/components/libraries/log \
  $(SDK_PATH)/components/libraries/hci \
  $(SDK_PATH)/components/boards \
  $(SDK_PATH)/components/libraries/bootloader \
  $(SDK_PATH)/components/libraries/crypto \
  $(SDK_PATH)/components/toolchain \
  $(SDK_PATH)/components/toolchain/cmsis/include \
  $(SDK_PATH)/external/micro-ecc/micro-ecc \
  $(SDK_PATH)/components/libraries/bootloader/ble_dfu \
  $(SDK_PATH)/external/nano-pb \
  $(SDK_PATH)/components/libraries/ecc \
  $(SDK_PATH)/components/softdevice/s130/headers/nrf51 \
  $(SDK_PATH)/components/libraries/queue \
  $(SDK_PATH)/components/toolchain/gcc \
  $(SDK_PATH)/components/drivers_nrf/rtc \
  
# Libraries common to all targets
LIB_FILES += \
  $(SDK_PATH)/external/micro-ecc/nrf51_armgcc/armgcc/micro_ecc_lib_nrf51.a \

# C flags common to all targets
CFLAGS += -DSWI_DISABLE0
CFLAGS += -DBOARD_PCA20006
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DNRF51
CFLAGS += -DNRF_DFU_SETTINGS_VERSION=1
CFLAGS += -D__HEAP_SIZE=0
CFLAGS += -DSVC_INTERFACE_CALL_AS_NORMAL_FUNCTION
CFLAGS += -DS130
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DNRF51822
CFLAGS += -DNRF_SD_BLE_API_VERSION=2
CFLAGS += -mcpu=cortex-m0
CFLAGS += -mthumb -mabi=aapcs
CFLAGS +=  -Wall -Werror -Os -g3
CFLAGS += -mfloat-abi=soft
# keep every function in separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums -flto

# C++ flags common to all targets
CXXFLAGS += \

# Assembler flags common to all targets
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -DBOARD_PCA20006
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DNRF51
ASMFLAGS += -DNRF_DFU_SETTINGS_VERSION=1
ASMFLAGS += -D__HEAP_SIZE=0
ASMFLAGS += -DSVC_INTERFACE_CALL_AS_NORMAL_FUNCTION
ASMFLAGS += -DS130
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DNRF51822
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=2

# Linker flags
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m0
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys


.PHONY: $(TARGETS) default all clean help flash 

# Default target - first one defined
default: bootloader_nrf51822_xxac_s130

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo 	bootloader_nrf51822_xxac_s130

TEMPLATE_PATH := $(SDK_PATH)/components/toolchain/gcc

include $(TEMPLATE_PATH)/Makefile.common
ifeq ($(OS),Windows_NT)
include $(TEMPLATE_PATH)/Makefile.windows
else
include $(TEMPLATE_PATH)/Makefile.posix
endif
$(foreach target, $(TARGETS), $(call define_target, $(target)))

# Flash the program
flash: $(OUTPUT_DIRECTORY)/bootloader_nrf51822_xxac_s130.hex
	@echo Flashing: $<
	nrfjprog --program $< -f nrf51 --sectorerase
	nrfjprog --reset -f nrf51

erase:
	nrfjprog --eraseall -f nrf51
