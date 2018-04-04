PROJECT_NAME := ble_network_beacon

export OUTPUT_FILENAME
#MAKEFILE_NAME := $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
MAKEFILE_NAME := $(MAKEFILE_LIST)
MAKEFILE_DIR := $(dir $(MAKEFILE_NAME) ) 
SDK_PATH = C:\nRF5_SDK_12.3.0_d7731ad
#SDK_PATH = D:/Privat/nRF5_SDK_11.0.0_89a8197
TEMPLATE_PATH = $(SDK_PATH)/components/toolchain/gcc

ifeq ($(OS),Windows_NT)
include $(TEMPLATE_PATH)/Makefile.windows
else
include $(TEMPLATE_PATH)/Makefile.posix
endif

MK := mkdir
RM := rm -rf

#echo suspend
ifeq ("$(VERBOSE)","1")
NO_ECHO := 
else
NO_ECHO := @
endif

# Toolchain commands
CC              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-gcc'
AS              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-as'
AR              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ar' -r
LD              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ld'
NM              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-nm'
OBJDUMP         := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objdump'
OBJCOPY         := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objcopy'
SIZE            := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-size'

#function for removing duplicates in a list
remduplicates = $(strip $(if $1,$(firstword $1) $(call remduplicates,$(filter-out $(firstword $1),$1))))

#source common to all targets
C_SOURCE_FILES += \
$(abspath   ../main.c) \
$(abspath   ../infection.c) \
$(abspath   ../network.c) \
$(abspath   ../radio.c) \
$(SDK_PATH)/components/toolchain/system_nrf51.c	\
$(SDK_PATH)/components/ble/common/ble_advdata.c	\
$(SDK_PATH)/components/libraries/util/app_error.c	\
$(SDK_PATH)/components/libraries/util/app_error_weak.c	\
$(SDK_PATH)/components/libraries/util/app_util_platform.c	\
$(SDK_PATH)/components/drivers_nrf/clock/nrf_drv_clock.c	\
$(SDK_PATH)/components/drivers_nrf/common/nrf_drv_common.c	\
$(SDK_PATH)/components/softdevice/common/softdevice_handler/softdevice_handler.c	\
$(SDK_PATH)/components/libraries/led_softblink/led_softblink.c \
$(SDK_PATH)/components/libraries/timer/app_timer.c \
$(SDK_PATH)/components/libraries/low_power_pwm/low_power_pwm.c \
$(SDK_PATH)/components/ble/ble_services/ble_nus/ble_nus.c \
$(SDK_PATH)/components/ble/ble_services/common/ble_conn_params.c \
$(SDK_PATH)/components/ble/ble_services/common/ble_srv_common.c \
$(SDK_PATH)/components/ble/ble_advertising/ble_advertising.c \
$(SDK_PATH)/components/libraries/fstorage/fstorage.c \
$(SDK_PATH)/components/drivers_nrf/power/nrf_drv_power.c \
#$(SDK_PATH)/components/libraries/experimental_section_vars/section_vars.c \

#assembly files common to all targets
ASM_SOURCE_FILES  = $(SDK_PATH)/components/toolchain/gcc/gcc_startup_nrf51.s
#$(SDK_PATH)
#includes common to all targets
#INC_PATHS += -I$(abspath ../../../../../components/device)
INC_PATHS += -I$(abspath   ../../Include)    #include folder for common_defines
INC_PATHS += -I$(abspath ../config/)
INC_PATHS += -I$(SDK_PATH)/components/device
INC_PATHS += -I$(SDK_PATH)/examples/bsp
INC_PATHS += -I$(SDK_PATH)/components/ble/common
INC_PATHS += -I$(SDK_PATH)/components/libraries/led_softblink
INC_PATHS += -I$(SDK_PATH)/components/drivers_nrf/hal
INC_PATHS += -I$(SDK_PATH)/components/drivers_nrf/delay
INC_PATHS += -I$(SDK_PATH)/components/libraries/timer
INC_PATHS += -I$(SDK_PATH)/components/libraries/util
INC_PATHS += -I$(SDK_PATH)/components/libraries/button
INC_PATHS += -I$(SDK_PATH)/components/toolchain/CMSIS/Include
INC_PATHS += -I$(SDK_PATH)/components/toolchain
INC_PATHS += -I$(SDK_PATH)/components/softdevice/s130/headers
INC_PATHS += -I$(SDK_PATH)/components/softdevice/common/softdevice_handler
INC_PATHS += -I$(SDK_PATH)/components/drivers_nrf/common
INC_PATHS += -I$(SDK_PATH)/components/drivers_nrf/clock
INC_PATHS += -I$(SDK_PATH)/components/libraries/low_power_pwm
INC_PATHS += -I$(SDK_PATH)/components/libraries/log
INC_PATHS += -I$(SDK_PATH)/components/libraries/log/src
INC_PATHS += -I$(SDK_PATH)/components/drivers_nrf/hal
INC_PATHS += -I$(SDK_PATH)/components/ble/ble_services/ble_nus
INC_PATHS += -I$(SDK_PATH)/components/ble/common
INC_PATHS += -I$(SDK_PATH)/components/ble/ble_advertising
INC_PATHS += -I$(SDK_PATH)/components/libraries/fstorage
INC_PATHS += -I$(SDK_PATH)/components/drivers_nrf/power
INC_PATHS += -I$(SDK_PATH)/components/libraries/experimental_section_vars

OBJECT_DIRECTORY = _build
LISTING_DIRECTORY = $(OBJECT_DIRECTORY)
OUTPUT_BINARY_DIRECTORY = $(OBJECT_DIRECTORY)

# Sorting removes duplicates
BUILD_DIRECTORIES := $(sort $(OBJECT_DIRECTORY) $(OUTPUT_BINARY_DIRECTORY) $(LISTING_DIRECTORY) )

#flags common to all targets
CFLAGS += -DuECC_VLI_NATIVE_LITTLE_ENDIAN=1
CFLAGS += -DNRF51
CFLAGS += -DuECC_SUPPORT_COMPRESSED_POINT=0
CFLAGS += -DuECC_SUPPORTS_secp256r1=1
CFLAGS += -DuECC_OPTIMIZATION_LEVEL=3
#changed uart to 0 for PCS10028 
#CFLAGS += -DNRF_LOG_USES_UART=0 
CFLAGS += -DuECC_SQUARE_FUNC=1
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -D__HEAP_SIZE=0
CFLAGS += -DuECC_ENABLE_VLI_API
CFLAGS += -DSWI_DISABLE0
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DS130
CFLAGS += -mcpu=cortex-m0
CFLAGS += -mthumb -mabi=aapcs --std=gnu99
CFLAGS += -mlong-calls
CFLAGS += -Wall -Werror -O3 -g3
CFLAGS += -mfloat-abi=soft
# keep every function in separate section. This will allow linker to dump unused functions
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums 
CFLAGS += -fstack-usage


# keep every function in separate section. This will allow linker to dump unused functions
LDFLAGS += -Xlinker -Map=$(LISTING_DIRECTORY)/$(OUTPUT_FILENAME).map
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m0
LDFLAGS += -mlong-calls
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys

# Assembler flags
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -DuECC_VLI_NATIVE_LITTLE_ENDIAN=1
ASMFLAGS += -DNRF51
ASMFLAGS += -DuECC_SUPPORT_COMPRESSED_POINT=0
ASMFLAGS += -DuECC_SUPPORTS_secp256r1=1
ASMFLAGS += -DuECC_OPTIMIZATION_LEVEL=3
#changed uart to 0 for PCS10028 
#ASMFLAGS += -DNRF_LOG_USES_UART=0
ASMFLAGS += -DuECC_SQUARE_FUNC=1
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -D__HEAP_SIZE=0
ASMFLAGS += -DuECC_ENABLE_VLI_API
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DS130

#default target - first one defined
default:  nrf51422_xxac_s130

#building all targets
all: 
#	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e cleanobj
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e nrf51822_xxac_s130
	
#target for printing all targets
help:
	@echo following targets are available:
	@echo 	nrf51422_xxac_s130
	@echo 	nrf51822_xxac_s130
	@echo 	flash_softdevice

C_SOURCE_FILE_NAMES = $(notdir $(C_SOURCE_FILES))
C_PATHS = $(call remduplicates, $(dir $(C_SOURCE_FILES) ) )
C_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(C_SOURCE_FILE_NAMES:.c=.o) )

ASM_SOURCE_FILE_NAMES = $(notdir $(ASM_SOURCE_FILES))
ASM_PATHS = $(call remduplicates, $(dir $(ASM_SOURCE_FILES) ))
ASM_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(ASM_SOURCE_FILE_NAMES:.s=.o) )

vpath %.c $(C_PATHS)
vpath %.s $(ASM_PATHS)

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)
LINKER_SCRIPT= ble_app_seuchen_gcc_nrf51.ld


nrf51822_xxac_s130: OUTPUT_FILENAME := nrf51822_xxac_s130
nrf51822_xxac_s130: CFLAGS += -DBOARD_PCA20006
nrf51822_xxac_s130: ASMFLAGS += -DBOARD_PCA20006
nrf51822_xxac_s130: CFLAGS += -DNRF51822
nrf51822_xxac_s130: ASMFLAGS += -DNRF51822
nrf51822_xxac_s130:  $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo Linking target: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -lm -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e finalize


nrf51422_xxac_s130: OUTPUT_FILENAME := nrf51422_xxac_s130
nrf51422_xxac_s130: CFLAGS += -DBOARD_PCA10028
nrf51422_xxac_s130: ASMFLAGS += -DBOARD_PCA10028
nrf51422_xxac_s130: CFLAGS += -DNRF51422
nrf51422_xxac_s130: ASMFLAGS += -DNRF51422
nrf51422_xxac_s130:  $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo Linking target: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -lm -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e finalize

nrfAOM_xxaa_s130: OUTPUT_FILENAME := nrfAOM_xxaa_s130
nrfAOM_xxaa_s130: CFLAGS += -DBOARD_AOM
nrfAOM_xxaa_s130: ASMFLAGS += -DBOARD_AOM
nrfAOM_xxaa_s130: CFLAGS += -DNRF51822
nrfAOM_xxaa_s130: ASMFLAGS += -DNRF51822
nrfAOM_xxaa_s130:  $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo Linking target: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -lm -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e finalize

## Create build directories
$(BUILD_DIRECTORIES):
	echo $(MAKEFILE_NAME)
	$(MK) $@

# Create objects from C SRC files
$(OBJECT_DIRECTORY)/%.o: %.c
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(CC) $(CFLAGS) $(INC_PATHS) -c -o $@ $<

# Assemble files
$(OBJECT_DIRECTORY)/%.o: %.s
	@echo Assembly file: $(notdir $<)
	$(NO_ECHO)$(CC) $(ASMFLAGS) $(INC_PATHS) -c -o $@ $<
# Link
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out: $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo Linking target: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -lm -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
## Create binary .bin file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	@echo Preparing: $(OUTPUT_FILENAME).bin
	$(NO_ECHO)$(OBJCOPY) -O binary $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	@echo Preparing: $(OUTPUT_FILENAME).hex
	$(NO_ECHO)$(OBJCOPY) -O ihex $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

finalize: genbin genhex echosize

genbin:
	@echo Preparing: $(OUTPUT_FILENAME).bin
	$(NO_ECHO)$(OBJCOPY) -O binary $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
genhex: 
	@echo Preparing: $(OUTPUT_FILENAME).hex
	$(NO_ECHO)$(OBJCOPY) -O ihex $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex
echosize:
	-@echo ''
	$(NO_ECHO)$(SIZE) $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	-@echo ''

clean:
	$(RM) $(BUILD_DIRECTORIES)

cleanobj:
	$(RM) $(BUILD_DIRECTORIES)/*.o
flash_beacon: nrf51822_xxac_s130
	@echo Flashing: $(OUTPUT_BINARY_DIRECTORY)/$<.hex
	nrfjprog --program $(OUTPUT_BINARY_DIRECTORY)/$<.hex -f nrf51  --sectorerase
	nrfjprog -p -f nrf51

flash_sd_beacon: nrf51822_xxac_s130
	@echo Flashing: s130_nrf51_2.0.0_softdevice.hex
	nrfjprog --program $(SDK_PATH)/components/softdevice/s130/hex/s130_nrf51_2.0.1_softdevice.hex -f nrf51 --chiperase
	nrfjprog --reset -f nrf51
	@echo Flashing: $(OUTPUT_BINARY_DIRECTORY)/$<.hex
	nrfjprog --program $(OUTPUT_BINARY_DIRECTORY)/$<.hex -f nrf51  --sectorerase
	nrfjprog -p -f nrf51
	
flash_DK: nrf51422_xxac_s130
	@echo Flashing: $(OUTPUT_BINARY_DIRECTORY)/$<.hex
	nrfjprog --program $(OUTPUT_BINARY_DIRECTORY)/$<.hex -f nrf51  --sectorerase
	nrfjprog -p -f nrf51
	
## Flash softdevice
flash_softdevice:
	@echo Flashing: s130_nrf51_2.0.0_softdevice.hex
	nrfjprog --program $(SDK_PATH)/components/softdevice/s130/hex/s130_nrf51_2.0.1_softdevice.hex -f nrf51 --chiperase
	nrfjprog -p -f nrf51