# SDK and Toolchain

This project targets Nordic nRF51 devices and is built with the Nordic nRF5 SDK
and GNU Arm Embedded toolchain.

## Required SDK

- Nordic nRF5 SDK: `12.3.0_d7731ad`
- Expected SDK path in the current Makefiles: `C:/nRF5_SDK_12.3.0_d7731ad`
- BLE SoftDevice: `S130`
- SoftDevice hex referenced by the Makefiles:
  `components/softdevice/s130/hex/s130_nrf51_2.0.1_softdevice.hex`
- BLE API version: `NRF_SD_BLE_API_VERSION=2`

The Makefiles use Nordic SDK build helpers from:

```text
$(SDK_PATH)/components/toolchain/gcc/Makefile.common
$(SDK_PATH)/components/toolchain/gcc/Makefile.windows
$(SDK_PATH)/components/toolchain/gcc/Makefile.posix
```

## Required Compiler

- GNU Arm Embedded GCC for ARM Cortex-M0
- The repository contains the historical installer
  `Obsoleszenz/gcc-arm-none-eabi-5_4-2016q3-20160926-win32.exe`.
- The Nordic SDK Makefiles expect the usual SDK toolchain variables from
  `components/toolchain/gcc/Makefile.windows` or `Makefile.posix`, especially:
  - `GNU_INSTALL_ROOT`
  - `GNU_PREFIX`

The firmware is compiled for Cortex-M0 with soft-float:

```text
-mcpu=cortex-m0
-mthumb
-mabi=aapcs
-mfloat-abi=soft
```

## Required Flashing Tools

- Nordic command line tools with `nrfjprog`
- SEGGER J-Link drivers

Historical installers are stored in `Obsoleszenz/`:

```text
nrf-command-line-tools-10.15.4-x86.exe
JLink_Windows_V766a_i386.exe
```

All flashing targets in the Makefiles use `nrfjprog -f nrf51`.

## Windows Setup

1. Install or unpack the Nordic nRF5 SDK to the path used by the Makefiles:

   ```text
   C:/nRF5_SDK_12.3.0_d7731ad
   ```

   If you install it elsewhere, update `SDK_PATH` in each Makefile.

2. Install GNU Arm Embedded GCC. The historical installer stored in this repo is:

   ```text
   Obsoleszenz/gcc-arm-none-eabi-5_4-2016q3-20160926-win32.exe
   ```

3. Configure the compiler path in the Nordic SDK Makefile:

   ```text
   C:/nRF5_SDK_12.3.0_d7731ad/components/toolchain/gcc/Makefile.windows
   ```

   Set `GNU_INSTALL_ROOT`, `GNU_VERSION`, and `GNU_PREFIX` for your installed
   compiler. For example:

   ```make
   GNU_INSTALL_ROOT := C:/Program Files (x86)/GNU Tools ARM Embedded/5.4 2016q3
   GNU_VERSION := 5.4.1
   GNU_PREFIX := arm-none-eabi
   ```

4. Install Nordic command line tools and SEGGER J-Link drivers so `nrfjprog`
   is available in a terminal.

5. Make sure `make` is available. The Nordic SDK Makefiles can be built from
   Git Bash, MSYS2, Cygwin, or another shell that provides GNU Make.

6. Build from the directory that contains the relevant Makefile. For example:

   ```powershell
   cd Network_Beacon/Makefile_Linker
   make
   ```

   To build a specific target:

   ```powershell
   make nrf51822_xxac_s130
   ```

   To see available targets:

   ```powershell
   make help
   ```

7. Flashing requires a connected Nordic board or beacon and working J-Link
   drivers. For example:

   ```powershell
   make flash_DK
   ```

   Some flash targets also program the SoftDevice before programming the
   application.

## Firmware Targets

### Network Beacon

Location: `Network_Beacon/Makefile_Linker/Makefile`

Default target:

```text
nrf51422_xxac_s130
```

Other declared targets include:

```text
nrf51822_xxac_s130
nrf51822_xxaa_s130
nrfAOM_xxaa_s130
```

Flash targets include:

```text
flash_beacon
flash_DK
flash_softdevice
```

### Network Base

Location: `Network_Base/Makefile_Linker/Makefile`

Default target:

```text
nrf51422_xxac
```

Flash targets include:

```text
flash_DK_Base
flash_softdevice
```

### Network Control

Location: `Network_Control/Makefile_Linker/Makefile`

Default target:

```text
nrf51422_xxac_s130
```

Flash targets include:

```text
flash_DK_Control
flash_softdevice
```

### DFU Bootloader

Location: `Network_Bootloader/pca10028/armgcc/Makefile`

Default target:

```text
bootloader_nrf51822_xxac_s130
```

Flash target:

```text
flash
```

The bootloader links against the SDK micro-ecc library:

```text
$(SDK_PATH)/external/micro-ecc/nrf51_armgcc/armgcc/micro_ecc_lib_nrf51.a
```

## Known Build Assumptions

- The project is pinned to nRF51 and SoftDevice S130.
- The Makefiles contain absolute Windows SDK paths. If the SDK is installed
  elsewhere, update `SDK_PATH` in the relevant Makefile.
- The repository is archival-style and includes old installers and reference
  documents under `Obsoleszenz/`.
- Build output is written to `_build` directories below each target folder.
