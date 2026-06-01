#include <zephyr/bluetooth/bluetooth.h>



struct device_id_entry {
    bt_addr_t addr;
    uint8_t id;
};

static const struct device_id_entry device_id_table[] = {
    {
        .addr = {
            .val = { 0xB1, 0x7D, 0x76, 0x1a, 0x92, 0xd1 }  // Developmentkit
        },
        .id = 1,
    },
};