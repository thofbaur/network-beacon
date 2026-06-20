## Domain Ownership

- `main.c` is orchestration only.
- `radio.c` owns Bluetooth transport, advertising/scanning, and command routing.
- `network.c` owns contact/proximity policy and network status.
- `nus.c` owns the data export interface.
- `led.c` owns LED behavior.
- `device.c` owns identity lookup and shared status-byte access.
- `param_storage.c` stays a generic persistence wrapper.
- `defines.h` is the shared protocol constant boundary.

# Architecture

## Intent

The firmware is organized around ownership boundaries. Each module owns one domain of behavior, while `main.c` only connects the domains during startup.

## Boundaries

## Startup

`main.c` controls initialization order. It should remain small and free of domain policy.

## Radio

The radio domain owns BLE advertising, scanning, command transport, and radio configuration. It may route events to other domains, but it should not own their rules.

## Network

The network domain owns proximity/contact policy, contact storage, and network status. Other modules may request or consume contact data through its public API.

## NUS

The NUS domain owns the Bluetooth data export interface. It should frame and send exported data, not decide how contacts are collected.

## Device

The device domain owns identity lookup and shared status-byte access. The meaning of each status bit or value remains with the domain that sets it.

## LED

The LED domain owns GPIO setup and visible LED behavior. Other modules should not manipulate LED hardware directly.

## Storage

The storage domain is intentionally generic. It should not know parameter meaning, defaults, validation, or reset behavior.

## Protocol Constants

Shared command IDs, base masks, and packet flags live in one place so protocol changes are explicit and reviewable.

## Concurrency

State shared between callbacks, work items, and connection handlers must have one clear owner and explicit synchronization.
