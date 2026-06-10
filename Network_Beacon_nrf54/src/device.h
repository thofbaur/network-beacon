#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <zephyr/bluetooth/addr.h>

uint8_t get_device_id(void);
uint8_t lookup_device_id(const bt_addr_le_t *addr);

uint8_t device_get_radio_status(void);
void device_set_radio_status(uint8_t status);
void device_set_radio_status_bit(uint8_t mask, bool active);

uint8_t device_get_network_status(void);
void device_set_network_status(uint8_t status);
