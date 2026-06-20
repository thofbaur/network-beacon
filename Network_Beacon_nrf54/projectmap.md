# Project Map

## Purpose

This firmware is a DSA network beacon for Zephyr/NCS on nRF54. It advertises its identity and status, scans nearby DSA devices, records proximity contacts, accepts configuration commands, and exports collected data over Bluetooth NUS.

## Core Domains

| Domain | Files | Intention |
| --- | --- | --- |
| Startup | `src/main.c` | Wire the system together and keep boot order obvious. |
| Radio/BLE | `src/radio.c`, `src/radio.h` | Own Bluetooth advertising, scanning, and command transport. |
| Network | `src/network.c`, `src/network.h` | Own proximity/contact rules, contact storage, and network status. |
| NUS | `src/nus.c`, `src/nus.h` | Own the data export interface. |
| Device | `src/device.c`, `src/device.h`, `src/radio_ids.h` | Own device identity and shared status bytes. |
| LED | `src/led.c`, `src/led.h` | Own user-visible LED behavior. |
| Storage | `src/param_storage.c`, `src/param_storage.h` | Provide generic persistence only. |
| Protocol constants | `src/defines.h` | Centralize shared command IDs and packet flags. |
| Platform/build | `CMakeLists.txt`, `prj.conf`, `boards/` | Configure Zephyr, Bluetooth, storage, GPIO, and board support. |

## High-Level Data Flow

1. The app initializes LED, network state, Bluetooth/radio, then starts advertising and scanning.
2. BLE advertisements expose the beacon identity and compact health/status information.
3. Scan results are classified as contact advertisements or command advertisements.
4. Contact advertisements are handed to the network domain for filtering, storage, and status updates.
5. Commands are routed to the domain that owns the affected parameter.
6. NUS clients can request collected data; the NUS domain exports data without owning contact policy.
7. Persistent settings are loaded and saved by each owning domain through the shared storage wrapper.

## Where Changes Belong

- Radio behavior, scan/advertise policy, and command transport: `src/radio.c`.
- Contact rules, contact format, network status, and proximity policy: `src/network.c`.
- NUS request/response behavior: `src/nus.c`.
- Identity mappings and status-byte access: `src/device.c` and `src/radio_ids.h`.
- LED behavior: `src/led.c`.
- Shared command IDs and protocol flags: `src/defines.h`.
- Generic storage plumbing only: `src/param_storage.c`.
- Boot sequencing only: `src/main.c`.

## Checks

There is no dedicated unit test suite yet. The minimum check for code changes is a Zephyr build:

```powershell
west build -b nrf54l15dk/nrf54l15/cpuapp/ns -d build_debug --sysbuild .
```

For incremental rebuilds:

```powershell
cmake --build build_debug
```

For BLE, NUS, storage, or contact-flow changes, also perform a focused hardware check and record what was verified.
