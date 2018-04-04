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
 * infection.h
 *
 *  Created on: 30.03.2018
 *      Author: thofbaur
 */


#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "nrf_error.h"
#include "ble_err.h"
#include "app_error.h"
#include "ble_nus.h"


#include "common_defines.h"
#include "beacon_common_defines.h"
#include "interface_defs.h"
#include "main_int.h"
#include "radio_int.h"

#ifdef SIMULATEINFECTION

#define INITIAL_STATUS				STATUS_S

#define INFECT_TIMEOUT			65 // Timeout for received adv. packet in seconds. 0..255 before infect contact status is reset. Compensate for "lost" pakets.
#define LIMIT_INFECT 				180 // Time needed with established contact for infection in seconds ToDo 240
#define INFECT_TIMEOUT_HEAL 		65 // Timeout for received adv. packet in seconds. 0..255 before infect contact status is reset. Compensate for "lost" pakets.

#define LIMIT_RECOVERY				21600 // Time to Recovery in seconds //TODO    6h (SIR-Modell)
#define LIMIT_LATENCY				5400 // ToDo in seconds 1500  / 2h
#define LIMIT_HEAL					300 // Time to heal in seconds TODO 300
#define LIMIT_SUSCEPT				1400000 // Time to suscept in seconds TODO (SIS-Modell)
#define LIMIT_RESET					3600 // immunity after reset in seconds 1800 //Maybe this can be changed as now a Infection version is implemented.
#define INFECT_LIMIT_RSSI		 			-80 // approx. 1-2m distance

#define INF_REV_INIT 				1

#endif
