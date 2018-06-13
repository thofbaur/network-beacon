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
 * network_int.h
 *
 *  Created on: 30.03.2018
 *      Author: thofbaur
 */

#ifndef NETWORKINT_H
#define NETWORKINT_H

#include "ble_nus.h"

void network_init();
void network_update_tag();
uint8_t network_nus_send_data(ble_nus_t * p_nus);
void network_evaluate_contact(const ble_gap_evt_t   * p_gap_evt);
void network_main(uint32_t *p_time_counter);
void network_control(uint8_t switch_param, uint8_t value1, uint8_t value2);
void network_set_tracking(uint8_t mode);
void network_save_params(void);
#endif
