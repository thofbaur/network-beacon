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


/*
 * radio.h
 *
 *  Created on: 30.03.2018
 *      Author: thofbaur
 */


#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "ble_gap.h"
#include "ble_nus.h"
#include "ble_hci.h"
#include "app_util.h"
#include "app_error.h"
#include "ble_conn_params.h"
#include "ble_advertising.h"
#include "app_timer.h"
#include "nrf_sdm.h"
#include "nrf_drv_config.h"  //board specific data, based on examples with additional Defines for LEDs
#include "softdevice_handler.h"


#include "common_defines.h"
#include "beacon_common_defines.h"
#include "interface_defs.h"
#include "main_int.h"
#include "network_int.h"
#include "infection_int.h"

#define TX_POWER					-20
#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(8, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(75, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */

#define APP_TIMER_PRESCALER             0    // Value of the RTC1 PRESCALER register.
#define APP_TIMER_OP_QUEUE_SIZE         10    // Size of timer operation queues.

//Additional defines necessary for connections
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define APP_FEATURE_NOT_SUPPORTED       BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */

#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN                  /**< UUID type for the Nordic UART Service (vendor specific). */

#define ADV_INTERVAL				250 // Advertisement interval in milliseconds
#define SCAN_WINDOW_MS				300  //scan window in milliseconds
#define SCAN_INTERVAL_MS			1000  // scan interval in milliseconds
#define CONNECTABLE_ADV_INTERVAL    MSEC_TO_UNITS(ADV_INTERVAL, UNIT_0_625_MS)
#define SCAN_WINDOW					MSEC_TO_UNITS(SCAN_WINDOW_MS, UNIT_0_625_MS)  //scan window in 0.625 mus
#define SCAN_INTERVAL				MSEC_TO_UNITS(SCAN_INTERVAL_MS, UNIT_0_625_MS)  // scan interval in 0.625 mus
//#define SCAN_PERIOD					20 //in seconds
#define SCAN_TIMEOUT				0

#define CENTRAL_LINK_COUNT          0                                  /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT       1                                  /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/
