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


struct beacon * tag_init(void);
void update_beacon_info(void);
void set_tag_status_infect(uint8_t status_infect);
uint8_t get_tag_status_infect(void);
void set_tag_inf_rev(uint8_t inf_rev);
uint8_t get_tag_inf_rev(void);
void set_tag_status_data(uint8_t *p_status_data);
void set_tag_status_batt(uint8_t status_batt);
uint8_t get_tag_status_batt(void);
uint8_t get_tag_id(void);



