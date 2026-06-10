#include <zephyr/bluetooth/bluetooth.h>
#include "radio_ids.h"
#include "device.h"

struct device_status {
    uint8_t radio;
    uint8_t network;
};

static struct device_status status_device;

uint8_t lookup_device_id(const bt_addr_le_t *addr)
{
    for (size_t i = 0; i < ARRAY_SIZE(known_device_table); i++) {
        if (bt_addr_le_cmp(addr, &known_device_table[i].addr) == 0) {
            return known_device_table[i].id;
        }
    }

    return 0xff; // unknown / unassigned
}

uint8_t get_device_id()
{
    static uint8_t device_id = 0xff;

    bt_addr_le_t addrs[CONFIG_BT_ID_MAX];
    size_t count = CONFIG_BT_ID_MAX;

    // Retrieve all addresses registered for the Bluetooth stack
    bt_id_get(addrs, &count);

    if (count > 0) {
		device_id = lookup_device_id(&addrs[0]);

        char addr_str[BT_ADDR_LE_STR_LEN];
        bt_addr_le_to_str(&addrs[0], addr_str, sizeof(addr_str));
	} else {
		printk("No Bluetooth identity address available, using unknown device id\n");
	}
	return device_id;
}

uint8_t device_get_radio_status(void)
{
    return status_device.radio;
}

void device_set_radio_status(uint8_t status)
{
    status_device.radio = status;
}

void device_set_radio_status_bit(uint8_t mask, bool active)
{
    if (active) {
        status_device.radio |= mask;
    } else {
        status_device.radio &= ~mask;
    }
}

uint8_t device_get_network_status(void)
{
    return status_device.network;
}

void device_set_network_status(uint8_t status)
{
    status_device.network = status;
}
