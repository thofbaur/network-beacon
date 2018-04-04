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
 * radio_int.h
 *
 *  Created on: 30.03.2018
 *      Author: thofbaur
 */

#include "ble_nus.h"


void scan_init(void);
void advertising_init(void);
void scan_start(void);
void advertising_start(void);
void gap_params_init(void);
void conn_params_init(void);
void ble_stack_init(void);
void services_init(void);
void radio_update_adv(uint8_t manuf_data[LENGTH_MANUF]);
uint8_t radio_nus_send(ble_nus_t * p_nus, uint8_t * p_string, uint16_t length);
