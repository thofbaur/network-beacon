/**
 *   Network-Beacon. Software to record the social network and simulate
 *   the spreading of an infection via BLE devices.
 *   Copyright (C) 2018  Tobias Hofbaur (tobias.hofbaur@gmx.de)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "fds.h"
#include "ble.h"
#include "ble_hci.h"
#include "nrf_drv_config.h"  //board specific data, ToDo Alex: Durch AOM Board Daten ersetzen.
#include "nrf_nvic.h"
#include "nrf_soc.h"
#include "app_timer.h"
#include "app_error.h"
#include "led_softblink.h"
#include "nrf_nvic.h"
#include "fstorage.h"

//#include "softdevice_handler.h"
//#define GPIO_COUNT 1
//#include "ble_nus.h"
//#include "ble_conn_params.h"
#include "ble_advertising.h"
//#include "ble_advdata.h"

#include "common_defines.h"
#include "beacon_common_defines.h"
#include "interface_defs.h"
#include "infection_int.h"
#include "network_int.h"
#include "radio_int.h"
#include "adjustable_params.h"


// General application timer settings.
#define APP_TIMER_PRESCALER             0    // Value of the RTC1 PRESCALER register.
#define APP_TIMER_OP_QUEUE_SIZE         10    // Size of timer operation queues.

#define BOOTLOADER_DFU_START 0xB1

//typedef  uint8_t mac_adress[6];
#define NUM_MACS	127
const uint8_t list_macs[127][6] = {
		{0x53,0xA1,0xC6,0xC8,0x48,0xF5}, //1
		{0x97,0x6A,0x7E,0x7E,0x08,0xED},
		{0xA7,0x33,0xC9,0x6A,0x31,0xD0},
		{0xFC,0x5B,0x41,0x07,0x3F,0xC1},
		{0x5A,0xEF,0x84,0x06,0xEB,0xDC},
		{0x9C,0xE8,0x8D,0x1C,0x6F,0xFA},
		{0x67,0x29,0x7F,0xCD,0x12,0xD9},
		{0x56,0xDC,0x3B,0x61,0xE5,0xCD},
		{0x12,0x19,0x5A,0x5D,0x6F,0xFF},
		{0xB9,0x04,0x87,0x06,0x5A,0xC8},//10
		{0xDB,0x80,0x70,0x96,0x19,0xEE},
		{0x41,0xAC,0x8D,0x2A,0x1E,0xF2},//12
		{0x74,0x70,0x93,0x24,0xE3,0xD3},
		{0xDA,0x6B,0x98,0xA9,0x99,0xE8},
		{0xA4,0x59,0xAE,0xB8,0x77,0xDC},
		{0x79,0x8E,0xE8,0x66,0x53,0xE0},
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//17
		{0x8E,0x41,0x75,0x25,0xAE,0xC9},
		{0xE9,0x03,0xE5,0xDF,0xE6,0xEC},
		{0x61,0xCD,0xAA,0xE4,0x06,0xC1},//20
		{0xE7,0x3D,0x67,0x2A,0xA0,0xDC},
		{0xFF,0x5A,0x9E,0x52,0x08,0xC5},
		{0xA1,0xD9,0xD1,0x21,0x97,0xDE},
		{0xE9,0x16,0x47,0xDA,0x8F,0xCC},
		{0xA4,0x27,0xF4,0x6F,0xD7,0xD8},
		{0x31,0x2C,0x25,0x49,0x28,0xCB},
		{0x32,0x8C,0x9B,0x99,0x5E,0xFF},
		{0x67,0xAC,0x2C,0x73,0xD5,0xE0},//
		{0x4C,0xF7,0xD6,0x16,0x53,0xF4},//29
		{0xF2,0x4F,0x63,0xA5,0x9F,0xEA},//30
		{0xE3,0xFA,0xC8,0x4D,0x17,0xFA},
		{0xF4,0x13,0x58,0x44,0x84,0xDF},
		{0xE4,0x85,0x46,0x23,0xAB,0xD1},
		{0x68,0x36,0x14,0x7B,0x17,0xFB},
		{0x51,0xB7,0xCC,0x6E,0xA6,0xE2},
		{0xA7,0x9B,0x0C,0xEF,0xBA,0xFA},
		{0x74,0xBC,0x8A,0xF9,0x78,0xE8},
		{0x3E,0x15,0x96,0x48,0x57,0xFA},
		{0xA8,0x42,0x33,0x0A,0xBA,0xF7},
		{0x44,0x43,0xF7,0xBC,0x1E,0xC5},//40
		{0xAB,0x09,0x0A,0xA5,0x99,0xF6},
		{0x20,0xDE,0xBF,0xED,0x27,0xC0},
		{0x4E,0x38,0xC8,0x28,0xDE,0xCA},
		{0xB6,0x1A,0x04,0xCD,0x20,0xD2},
		{0xA0,0xC1,0x8C,0x69,0x44,0xF1},
		{0x22,0xB9,0x6B,0xEF,0xB5,0xDC},
		{0xF2,0x77,0xCE,0x25,0x3B,0xCC},
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//48
		{0x73,0xC7,0x3E,0xD1,0x39,0xC0},
		{0xA2,0xD9,0xE9,0x92,0x7E,0xF6},//50
		{0x79,0x87,0x44,0x47,0x22,0xD8},
		{0x49,0xA5,0xCD,0xAB,0xE2,0xD1},
		{0x29,0x7C,0x61,0xD9,0x51,0xD9},
		{0x3C,0x2F,0x3A,0xCC,0x0E,0xE4},
		{0xB6,0x19,0xBA,0xDD,0x68,0xDA},
		{0xAA,0x93,0x2E,0xA3,0x73,0xF4},
		{0x72,0xF1,0x5D,0x45,0xEE,0xD6},
		{0x6F,0x2C,0x8E,0x61,0xF3,0xED},
		{0x7A,0x08,0xA4,0x11,0x29,0xE8},
		{0x11,0x2D,0x31,0x88,0x38,0xF2},//60
		{0xB3,0xB0,0x8D,0xCB,0x4D,0xC5},
		{0x72,0x0E,0x5D,0x5E,0x93,0xF9},
		{0xA8,0xF6,0x18,0x1C,0x10,0xF9},
		{0x8E,0x0D,0x50,0xE7,0xCE,0xE8},
		{0xAC,0x42,0x2B,0x3D,0x9F,0xE2},
		{0x81,0x1B,0x5E,0x8B,0x10,0xF2},
		{0x4B,0x5F,0xBF,0xA8,0x19,0xED},
		{0x21,0xF5,0xD2,0xA2,0x63,0xDA},
		{0x9F,0xC0,0xDF,0x8E,0x60,0xF7},
		{0x70,0x99,0x30,0x05,0xA7,0xF1},//70
		{0x63,0x45,0xB4,0xDA,0x4E,0xF4},
		{0x5D,0x89,0xB4,0x63,0x42,0xCB},
		{0xF9,0x72,0xAC,0x60,0x85,0xD8},
		{0x13,0x46,0x09,0x73,0x5E,0xF3},
		{0x1E,0x67,0x4D,0xE5,0x4F,0xC5},
		{0x53,0x84,0x15,0x7B,0x86,0xF6},
		{0x92,0x1D,0xE6,0x97,0x35,0xDB},
		{0x4A,0x69,0x97,0x05,0x96,0xFF},
		{0x33,0x74,0x37,0x0F,0x11,0xE3},
		{0x4F,0xB5,0xDA,0x42,0xAE,0xC2},//80
		{0xD0,0xEE,0x97,0xE2,0xE3,0xE6},
		{0x36,0x60,0x59,0xD7,0xC3,0xF8},
		{0xD9,0xD7,0x12,0x68,0x03,0xC2},
		{0xB5,0x0C,0x91,0xBC,0xCA,0xC0},
		{0x97,0x5E,0x9E,0xB1,0x65,0xC1},
		{0x69,0x14,0xCC,0xFC,0x17,0xD3},
		{0x1E,0xFD,0xC4,0x36,0x3A,0xD4},
		{0x64,0xD2,0x57,0xD0,0x91,0xCB},
		{0x89,0x59,0xFD,0xB6,0x4B,0xC8},
		{0xD7,0x9D,0x0A,0x07,0xC7,0xC7},//90
		{0x3E,0xB1,0x0E,0x46,0x4B,0xE0},
		{0x0E,0x7E,0x0E,0x8A,0xF1,0xDD},
		{0xB4,0x5A,0x77,0x4E,0xCE,0xE5},
		{0x4D,0xF1,0xEB,0x73,0x91,0xEE},
		{0xDB,0x84,0x6F,0x33,0x35,0xEC},
		{0x02,0x65,0xE9,0x5C,0xF6,0xEB},//
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//97
		{0x48,0xFC,0x40,0x81,0xDA,0xF4},//
		{0xCB,0x34,0x3D,0xCA,0x86,0xF5},
		{0x45,0x53,0xE5,0xC6,0x7C,0xDF},//100
		{0x36,0xF6,0xFA,0xFE,0x92,0xDB},
		{0x99,0x7B,0xDD,0xCB,0xCC,0xF8},
		{0x8F,0xA7,0x27,0xEA,0x4F,0xE6},
		{0x2D,0xFF,0x12,0xB9,0x17,0xC8},
		{0x05,0x92,0x09,0xDB,0xD7,0xC9},
		{0x5E,0x22,0x3A,0x93,0x85,0xDE},
		{0xED,0xE8,0x7A,0xA5,0xCC,0xC6},
		{0x11,0x94,0x0A,0x56,0x66,0xE7},
		{0x4F,0xAB,0xA9,0xF5,0x19,0xCD},
		{0x57,0xEF,0x88,0x93,0x37,0xD6},//110
		{0xF2,0xD9,0x18,0xEA,0x7A,0xC5},
		{0xE8,0xBE,0xA3,0xC3,0x9D,0xE3},
		{0x3D,0x25,0x71,0x4A,0x46,0xC5},
		{0xE2,0xEB,0x05,0x15,0x1A,0xE4},
		{0xA8,0xD5,0x1A,0x76,0x2E,0xF7},
		{0x53,0x7E,0x23,0xD5,0xD6,0xE2},
		{0x91,0xA0,0x7C,0x45,0x60,0xD5},
		{0xBD,0x5D,0xA2,0x44,0xCE,0xD0},//118
		{0x61,0xb2,0x74,0x6b,0x69,0xD2}, // DK
		{0x8D,0x95,0xE0,0x2C,0xAB,0xC6}, // DK
		{0xFB,0x3A,0xE1,0xE2,0xD7,0xCF}, // DK
		{0x07,0xCF,0x19,0xF1,0x5E,0xE3} // DK
};
