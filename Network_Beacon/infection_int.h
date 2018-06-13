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
 * infection_int.h
 *
 *  Created on: 30.03.2018
 *      Author: thofbaur
 */

#ifdef SIMULATEINFECTION

#ifndef INFECTIONINT_H
#define INFECTIONINT_H

#include "ble_nus.h"



void infect_init(struct beacon *tag);
void status_change(uint8_t status_new,struct beacon *tag,uint32_t *time_counter);
uint8_t infect_nus_send_data(ble_nus_t * p_nus);
void infect_evaluate_contact(struct beacon *tag,const ble_gap_evt_t   * p_gap_evt);
void infect_main(struct beacon *tag,uint32_t *time_counter);
void infect_control(uint8_t switch_param, uint8_t value1, uint8_t value2,struct beacon *p_tag,uint32_t *p_time_counter);
void infect_reset_idx_read(void);
void infect_set_infect(uint8_t mode);
void infect_save_params(void);

#endif
#endif
