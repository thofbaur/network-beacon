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
#ifdef SIMULATEINFECTION
#ifndef IDLIST
#error "Simulate infection only works with pre defined ID-List"
#endif
#endif


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

#include "adjustable_params.h"

