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
#include "interface_defs.h"
#include "infection_int.h"
#include "network_int.h"
#include "radio_int.h"
#include "adjustable_params.h"
#include "led_int.h"
#include "beacon_status_int.h"


// General application timer settings.
#define APP_TIMER_PRESCALER             0    // Value of the RTC1 PRESCALER register.
#define APP_TIMER_OP_QUEUE_SIZE         10    // Size of timer operation queues.

#define BOOTLOADER_DFU_START 0xB1


