#include <zephyr/bluetooth/bluetooth.h>



struct known_device {
    bt_addr_le_t addr;
    uint8_t id;
};

static const struct known_device known_device_table[] = {
    {
        .addr = {
            .type = BT_ADDR_LE_RANDOM,
            .a = { .val = { 0xB1, 0x7D, 0x76, 0x1a, 0x92, 0xd1 } }  // Developmentkit
        },
        .id = 1,
    },
    {
        .addr = {
            .type = BT_ADDR_LE_RANDOM,
            .a = { .val = { 0x2D, 0xFF, 0x12, 0xB9, 0x17, 0xC8 } }  // altes nrf51822 beacon
        },
        .id = 104,
    },
    {
        .addr = {
            .type = BT_ADDR_LE_RANDOM,
            .a = { .val = { 0xFB, 0x3A, 0xE1, 0xE2, 0xD7, 0xCF } }  // altes nrf51422 dev kit
        },
        .id = 0,
    },
};
