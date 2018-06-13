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


void set_status_led(void);
void sys_evt_dispatch(uint32_t sys_evt);
void evaluate_adv_report(const ble_gap_evt_t   * p_gap_evt);
uint8_t main_nus_send_time(ble_nus_t * p_nus);
void update_beacon_info(void);
void update_tag_status_infect(uint8_t status_infect);
void update_tag_inf_rev(uint8_t inf_rev);
void update_tag_status_data(uint8_t *p_status_data);
void main_reset_time_sent(void);
bool main_record_exists(uint16_t file_id, uint16_t key);
void main_save_data(void *p_data, uint8_t length, uint16_t file_id, uint16_t key);
void main_read_data(void *p_data, uint8_t length, uint16_t file_id, uint16_t key);



