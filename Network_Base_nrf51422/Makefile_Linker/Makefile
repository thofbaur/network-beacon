PROJECT_NAME     := ble_app_netzwerk_zentrale_pca10028_s130
TARGETS          := nrf51422_xxac
OUTPUT_DIRECTORY := _build


#SDK_PATH = C:/nRF5_SDK_12.3.0_d7731ad
SDK_PATH = D:/nRF5_SDK_12.3.0_d7731ad
TEMPLATE_PATH = $(SDK_PATH)/components/toolchain/gcc

PROJ_DIR := ..

$(OUTPUT_DIRECTORY)/nrf51422_xxac.out: \
  LINKER_SCRIPT  := ble_app_netzwerk_zentrale_gcc_nrf51.ld

# Source files common to all targets
SRC_FILES += \
  $(SDK_PATH)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_PATH)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_PATH)/components/libraries/button/app_button.c \
  $(SDK_PATH)/components/libraries/util/app_error.c \
  $(SDK_PATH)/components/libraries/util/app_error_weak.c \
  $(SDK_PATH)/components/libraries/fifo/app_fifo.c \
  $(SDK_PATH)/components/libraries/timer/app_timer.c \
  $(SDK_PATH)/components/libraries/uart/app_uart_fifo.c \
  $(SDK_PATH)/components/libraries/util/app_util_platform.c \
  $(SDK_PATH)/components/libraries/hardfault/hardfault_implementation.c \
  $(SDK_PATH)/components/libraries/util/nrf_assert.c \
  $(SDK_PATH)/components/libraries/uart/retarget.c \
  $(SDK_PATH)/components/libraries/util/sdk_errors.c \
  $(SDK_PATH)/components/boards/boards.c \
  $(SDK_PATH)/components/drivers_nrf/clock/nrf_drv_clock.c \
  $(SDK_PATH)/components/drivers_nrf/common/nrf_drv_common.c \
  $(SDK_PATH)/components/drivers_nrf/gpiote/nrf_drv_gpiote.c \
  $(SDK_PATH)/components/drivers_nrf/uart/nrf_drv_uart.c \
  $(SDK_PATH)/components/libraries/bsp/bsp.c \
  $(SDK_PATH)/components/libraries/bsp/bsp_btn_ble.c \
  $(SDK_PATH)/components/libraries/bsp/bsp_nfc.c \
  $(PROJ_DIR)/main.c \
  $(SDK_PATH)/external/segger_rtt/RTT_Syscalls_GCC.c \
  $(SDK_PATH)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_PATH)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_PATH)/components/ble/common/ble_advdata.c \
  $(SDK_PATH)/components/ble/common/ble_conn_params.c \
  $(SDK_PATH)/components/ble/ble_db_discovery/ble_db_discovery.c \
  $(SDK_PATH)/components/ble/common/ble_srv_common.c \
  $(SDK_PATH)/components/toolchain/gcc/gcc_startup_nrf51.S \
  $(SDK_PATH)/components/toolchain/system_nrf51.c \
  $(SDK_PATH)/components/ble/ble_services/ble_nus_c/ble_nus_c.c \
  $(SDK_PATH)/components/softdevice/common/softdevice_handler/softdevice_handler.c \

# Include folders common to all targets
INC_FOLDERS += \
  $(SDK_PATH)/components/drivers_nrf/comp \
  $(SDK_PATH)/components/drivers_nrf/twi_master \
  $(SDK_PATH)/components/ble/ble_services/ble_ancs_c \
  $(SDK_PATH)/components/ble/ble_services/ble_ias_c \
  $(SDK_PATH)/components/softdevice/s130/headers \
  $(SDK_PATH)/components/libraries/pwm \
  $(SDK_PATH)/components/libraries/usbd/class/cdc/acm \
  $(SDK_PATH)/components/libraries/usbd/class/hid/generic \
  $(SDK_PATH)/components/libraries/usbd/class/msc \
  $(SDK_PATH)/components/libraries/usbd/class/hid \
  $(SDK_PATH)/components/libraries/log \
  $(SDK_PATH)/components/ble/ble_services/ble_gls \
  $(SDK_PATH)/components/libraries/fstorage \
  $(SDK_PATH)/components/drivers_nrf/i2s \
  $(SDK_PATH)/components/libraries/gpiote \
  $(SDK_PATH)/components/drivers_nrf/gpiote \
  $(SDK_PATH)/components/libraries/fifo \
  $(SDK_PATH)/components/boards \
  $(SDK_PATH)/components/drivers_nrf/common \
  $(SDK_PATH)/components/ble/ble_advertising \
  $(SDK_PATH)/components/drivers_nrf/adc \
  $(SDK_PATH)/components/softdevice/s130/headers/nrf51 \
  $(SDK_PATH)/components/ble/ble_services/ble_bas_c \
  $(SDK_PATH)/components/ble/ble_services/ble_hrs_c \
  $(SDK_PATH)/components/libraries/queue \
  $(SDK_PATH)/components/ble/ble_dtm \
  $(SDK_PATH)/components/toolchain/cmsis/include \
  $(SDK_PATH)/components/ble/ble_services/ble_rscs_c \
  $(SDK_PATH)/components/drivers_nrf/uart \
  $(SDK_PATH)/components/ble/common \
  $(SDK_PATH)/components/ble/ble_services/ble_lls \
  $(SDK_PATH)/components/drivers_nrf/wdt \
  $(SDK_PATH)/components/libraries/bsp \
  $(SDK_PATH)/components/ble/ble_db_discovery \
  $(SDK_PATH)/components/ble/ble_services/ble_bas \
  $(SDK_PATH)/components/libraries/experimental_section_vars \
  $(SDK_PATH)/components/ble/ble_services/ble_ans_c \
  $(SDK_PATH)/components/libraries/slip \
  $(SDK_PATH)/components/libraries/mem_manager \
  $(SDK_PATH)/external/segger_rtt \
  $(SDK_PATH)/components/libraries/usbd/class/cdc \
  $(SDK_PATH)/components/drivers_nrf/hal \
  $(SDK_PATH)/components/ble/ble_services/ble_nus_c \
  $(SDK_PATH)/components/drivers_nrf/rtc \
  $(SDK_PATH)/components/ble/ble_services/ble_ias \
  $(SDK_PATH)/components/libraries/usbd/class/hid/mouse \
  $(SDK_PATH)/components/drivers_nrf/ppi \
  $(SDK_PATH)/components/ble/ble_services/ble_dfu \
  $(SDK_PATH)/components/drivers_nrf/twis_slave \
  $(SDK_PATH)/components \
  $(SDK_PATH)/components/libraries/scheduler \
  $(SDK_PATH)/components/ble/ble_services/ble_lbs \
  $(SDK_PATH)/components/ble/ble_services/ble_hts \
  $(SDK_PATH)/components/drivers_nrf/delay \
  $(SDK_PATH)/components/libraries/crc16 \
  $(SDK_PATH)/components/drivers_nrf/timer \
  $(SDK_PATH)/components/libraries/util \
  $(SDK_PATH)/components/drivers_nrf/pwm \
  ../config \
  ../../Include/ \
  $(SDK_PATH)/components/libraries/csense_drv \
  $(SDK_PATH)/components/libraries/csense \
  $(SDK_PATH)/components/drivers_nrf/rng \
  $(SDK_PATH)/components/libraries/low_power_pwm \
  $(SDK_PATH)/components/libraries/hardfault \
  $(SDK_PATH)/components/ble/ble_services/ble_cscs \
  $(SDK_PATH)/components/libraries/uart \
  $(SDK_PATH)/components/libraries/hci \
  $(SDK_PATH)/components/libraries/usbd/class/hid/kbd \
  $(SDK_PATH)/components/drivers_nrf/spi_slave \
  $(SDK_PATH)/components/drivers_nrf/lpcomp \
  $(SDK_PATH)/components/libraries/timer \
  $(SDK_PATH)/components/drivers_nrf/power \
  $(SDK_PATH)/components/libraries/usbd/config \
  $(SDK_PATH)/components/toolchain \
  $(SDK_PATH)/components/libraries/led_softblink \
  $(SDK_PATH)/components/drivers_nrf/qdec \
  $(SDK_PATH)/components/ble/ble_services/ble_cts_c \
  $(SDK_PATH)/components/drivers_nrf/spi_master \
  $(SDK_PATH)/components/ble/ble_services/ble_nus \
  $(SDK_PATH)/components/ble/ble_services/ble_hids \
  $(SDK_PATH)/components/drivers_nrf/pdm \
  $(SDK_PATH)/components/libraries/crc32 \
  $(SDK_PATH)/components/libraries/usbd/class/audio \
  $(SDK_PATH)/components/ble/peer_manager \
  $(SDK_PATH)/components/drivers_nrf/swi \
  $(SDK_PATH)/components/ble/ble_services/ble_tps \
  $(SDK_PATH)/components/ble/ble_services/ble_dis \
  $(SDK_PATH)/components/device \
  $(SDK_PATH)/components/ble/nrf_ble_qwr \
  $(SDK_PATH)/components/libraries/button \
  $(SDK_PATH)/components/libraries/usbd \
  $(SDK_PATH)/components/drivers_nrf/saadc \
  $(SDK_PATH)/components/ble/ble_services/ble_lbs_c \
  $(SDK_PATH)/components/ble/ble_racp \
  $(SDK_PATH)/components/toolchain/gcc \
  $(SDK_PATH)/components/libraries/fds \
  $(SDK_PATH)/components/libraries/twi \
  $(SDK_PATH)/components/drivers_nrf/clock \
  $(SDK_PATH)/components/ble/ble_services/ble_rscs \
  $(SDK_PATH)/components/drivers_nrf/usbd \
  $(SDK_PATH)/components/softdevice/common/softdevice_handler \
  $(SDK_PATH)/components/ble/ble_services/ble_hrs \
  $(SDK_PATH)/components/libraries/log/src \

# Libraries common to all targets
LIB_FILES += \

# C flags common to all targets
CFLAGS += -DBOARD_PCA10028
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DNRF51
CFLAGS += -D__HEAP_SIZE=0
CFLAGS += -DS130
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DSWI_DISABLE0
CFLAGS += -DBSP_UART_SUPPORT
CFLAGS += -DNRF51422
CFLAGS += -DNRF_SD_BLE_API_VERSION=2
CFLAGS += -mcpu=cortex-m0
CFLAGS += -mthumb -mabi=aapcs
CFLAGS +=  -Wall -Werror -O0 -g3
CFLAGS += -mfloat-abi=soft
# keep every function in separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums

# C++ flags common to all targets
CXXFLAGS += \

# Assembler flags common to all targets
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -DBOARD_PCA10028
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DNRF51
ASMFLAGS += -D__HEAP_SIZE=0
ASMFLAGS += -DS130
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -DBSP_UART_SUPPORT
ASMFLAGS += -DNRF51422
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=2

# Linker flags
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m0
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys


.PHONY: $(TARGETS) default all clean help flash_DK_Base flash_softdevice erase

# Default target - first one defined
default: nrf51422_xxac

all: nrf51422_xxac

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo 	nrf51422_xxac
	@echo 	flash_DK_Base
	@echo 	flash_softdevice
	@echo 	erase

TEMPLATE_PATH := $(SDK_PATH)/components/toolchain/gcc

GNU_PREFIX ?= arm-none-eabi
ifeq ($(OS),Windows_NT)
ifeq ($(origin GNU_INSTALL_ROOT), undefined)
GNU_GCC_ON_PATH := $(shell powershell -NoProfile -Command "$$cmd = Get-Command $(GNU_PREFIX)-gcc -ErrorAction SilentlyContinue; if ($$cmd) { $$cmd.Source -replace '\\','/' }")
ifneq ($(strip $(GNU_GCC_ON_PATH)),)
override GNU_INSTALL_ROOT := $(patsubst %/bin/$(GNU_PREFIX)-gcc.exe,%,$(GNU_GCC_ON_PATH))
override GNU_VERSION := $(shell "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-gcc" -dumpversion)
endif
endif
endif

include $(TEMPLATE_PATH)/Makefile.common

ifeq ($(OS),Windows_NT)
RM := powershell -NoProfile -Command "& { Remove-Item -LiteralPath $$args -Recurse -Force -ErrorAction SilentlyContinue }"
endif

$(foreach target, $(TARGETS), $(call define_target, $(target)))

# Flash the program
flash_DK_Base: $(OUTPUT_DIRECTORY)/nrf51422_xxac.hex
	@echo Flashing: $<
	nrfjprog --program $< -f nrf51 --sectorerase
	nrfjprog --reset -f nrf51

# Flash softdevice
flash_softdevice:
	@echo Flashing: s130_nrf51_2.0.1_softdevice.hex
	nrfjprog --program $(SDK_PATH)/components/softdevice/s130/hex/s130_nrf51_2.0.1_softdevice.hex -f nrf51 --sectorerase
	nrfjprog --reset -f nrf51

erase:
	nrfjprog --eraseall -f nrf51
