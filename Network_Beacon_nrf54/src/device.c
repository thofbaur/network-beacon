#include <zephyr/bluetooth/bluetooth.h>
#include "radio_ids.h"




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
    static uint8_t device_id;

    bt_addr_le_t addrs[CONFIG_BT_ID_MAX];
    size_t count = CONFIG_BT_ID_MAX;

    // Retrieve all addresses registered for the Bluetooth stack
    bt_id_get(addrs, &count);
	device_id = lookup_device_id(&addrs[0]);

    if (count > 0) {
        char addr_str[BT_ADDR_LE_STR_LEN];
        bt_addr_le_to_str(&addrs[0], addr_str, sizeof(addr_str));
        printk("Device Address: %s\n", addr_str);
    
	
		printk("Raw bytes: %02x %02x %02x %02x %02x %02x\n",
       	addrs[0].a.val[0], addrs[0].a.val[1], addrs[0].a.val[2],
       	addrs[0].a.val[3], addrs[0].a.val[4], addrs[0].a.val[5]);
	}
	return device_id;
}