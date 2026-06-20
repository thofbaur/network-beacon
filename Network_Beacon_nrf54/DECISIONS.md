# Decisions

Short records of design intent. Add entries when the reason for a choice would not be obvious from the code.

## 2026-06-20: Keep Startup Thin

`main.c` should only initialize and start the system. Domain behavior belongs in owner modules.

## 2026-06-20: Route Commands Through Radio, Execute In Owner Domains

Commands arrive through BLE scanning, so radio owns transport and routing. The affected domain owns validation, behavior, and persistence.

## 2026-06-20: Keep Persistence Domain-Owned

Each domain owns its defaults and parameter meaning. The storage layer remains a generic load/save wrapper.

## 2026-06-20: Keep Advertised Status Compact

Advertisements expose only compact identity and status information so nearby devices can inspect state without a connection.

## 2026-06-20: Treat Contact Export Format As Protocol

The contact export shape is part of the external protocol. Changes require an explicit compatibility decision.

## 2026-06-20: Known Device Identity Is Centralized

Device identity mappings are kept in one place so scan filtering and local ID lookup stay aligned.

## 2026-06-20: NUS Export Drains Stored Contacts

The current export model treats a successful readout as consuming stored contact data.
