/**
 *   Network_Control. Software to record the social network and simulate
 *   the spreading of an infection via BLE devices.
 *   Copyright (C) 2018  Tobias Hofbaur (tobias.hofbaur @ gmx.de)
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

#include "nrf_drv_config.h"  //board specific data, based on examples with additional Defines for LEDs

#include "app_timer.h"
#include "app_error.h"
#include "softdevice_handler.h"
#define GPIO_COUNT 1
#include "led_softblink.h"


#include "common_defines.h"



#define CENTRAL_LINK_COUNT          0                                  /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT       1                                  /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define ADV_INTERVAL				100 // Advertisement interval in milliseconds
#define CONNECTABLE_ADV_INTERVAL    MSEC_TO_UNITS(ADV_INTERVAL, UNIT_0_625_MS)
#define TX_POWER					4


// General application timer settings.
#define APP_TIMER_PRESCALER             0    // Value of the RTC1 PRESCALER register.
#define APP_TIMER_OP_QUEUE_SIZE         10    // Size of timer operation queues.
