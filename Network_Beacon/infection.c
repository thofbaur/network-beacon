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
 * infection.c
 *
 *  Created on: 30.03.2018
 *      Author: thofbaur
 */


#include "infection.h"
#ifdef SIMULATEINFECTION
uint8_t infect_array[LENGTH_INFECT_ARRAY][WIDTH_INFECT_ARRAY];

uint8_t infect_source[WIDTH_INFECT_ARRAY];
uint32_t infect_time;

static uint32_t timer_state = 0;
static uint16_t limit_time_infect = LIMIT_INFECT; //10 Minuten
static uint32_t limit_time_recovery = LIMIT_RECOVERY;
static uint32_t limit_time_latency = LIMIT_LATENCY;
static uint32_t limit_time_heal = LIMIT_HEAL;
static uint32_t limit_time_suscept = LIMIT_SUSCEPT;
static uint16_t timer_reset = LIMIT_RESET;

static uint16_t limit_timeout_contact_infect = INFECT_TIMEOUT; // 65 sekunden
static uint16_t limit_timeout_kontakt_heal = 2*INFECT_TIMEOUT; // 65 sekunden

uint8_t idx_infect_read=0;
static uint8_t kontakt_infect = 0;
static uint8_t kontakt_heal = 0;

void reset_source(void)
{
	memset(infect_source,0x00,WIDTH_INFECT_ARRAY-3);
}

void write_infect_array(struct beacon *tag)
{
	uint8_t i;
	static uint8_t idx_infect=0;

	infect_array[idx_infect][0] = tag->status_infect;
	infect_array[idx_infect][1] = infect_time >>13 & 0xff;
	infect_array[idx_infect][2] = infect_time >>5  & 0xff;
	for(i=0;i<WIDTH_INFECT_ARRAY-3;i++)
	{
		infect_array[idx_infect][3+i] = infect_source[i];
	}
	idx_infect +=1;
	if(idx_infect == LENGTH_INFECT_ARRAY)
	{
		idx_infect = 0;
	}
}

void infect_init(struct beacon *tag)
{
	uint8_t inf_rev_init = INF_REV_INIT;
	infect_time = TIME_ZERO;
	update_tag_status_infect(INITIAL_STATUS);
	update_tag_inf_rev(&inf_rev_init);
	reset_source();
	write_infect_array(tag);
}


void status_change(uint8_t status_new,struct beacon *tag, uint32_t *time_counter)
{
//	uint8_t i;
//    uint32_t       err_code;

	if(tag->status_infect == status_new)
	{
	}else
	{
		infect_time = *time_counter;
		update_tag_status_infect(STATUS_S);
		write_infect_array(tag);
		reset_source();
		set_status_led();
		update_beacon_info();
		timer_state = 0;
	}
}



void add_source(uint8_t tag_id)
{
	infect_source[(tag_id-1) >> 3] |=  (1<< ( 7-( (tag_id-1)&7) ) );
}

uint8_t infect_nus_send_data(uint16_t *nus_cnt,ble_nus_t * p_nus)
{
	uint8_t result_send = 0;
	uint8_t infect_sent = 0;
    if (idx_infect_read == LENGTH_INFECT_ARRAY)
	{
		infect_sent = 1;
	}
    else
    {
    	infect_sent = 0;
    }
    while( !infect_sent )
    {
		result_send = radio_nus_send(p_nus, infect_array[idx_infect_read],WIDTH_INFECT_ARRAY);
    	if(result_send)
    	{
			idx_infect_read++;
			if (idx_infect_read == LENGTH_INFECT_ARRAY)
			{
				infect_sent = 1;
				idx_infect_read = 0;
			}

		}
    }
    return result_send;
}


void infect_evaluate_contact(struct beacon *tag,const ble_gap_evt_t   * p_gap_evt)
{
	static int8_t infect_limit_rssi = INFECT_LIMIT_RSSI;

	if( p_gap_evt->params.adv_report.rssi >= infect_limit_rssi)
	{

		if(  (p_gap_evt->params.adv_report.data[POS_INF_STATUS] & INFECT_MASK) == STATUS_I && \
			(p_gap_evt->params.adv_report.data[POS_INF_STATUS] & INFECT_REV_MASK) == tag->inf_rev && tag->status_infect == STATUS_S)
		{
			kontakt_infect = 1;
			add_source(p_gap_evt->params.adv_report.data[POS_ID]);
		}
		if(  (p_gap_evt->params.adv_report.data[POS_INF_STATUS] & INFECT_MASK) == STATUS_H && \
			(p_gap_evt->params.adv_report.data[POS_INF_STATUS] & INFECT_REV_MASK) == tag->inf_rev && tag->status_infect == STATUS_I)
		{
			kontakt_heal = 1;
		}
	}
}
void infect_main(struct beacon *tag,uint32_t *time_counter)
{
	static uint32_t time_kontakt = INFECT_TIMEOUT;
	static uint32_t time_kontakt_heal = INFECT_TIMEOUT_HEAL;
	static uint32_t timer_infect = 0;
	static uint32_t timer_heal = 0;

	timer_state += MAIN_SAMPLE_RATE;

	//deactivate Beacon after reset from Admin Beacon
	if( timer_reset < LIMIT_RESET)
	{
		timer_reset += MAIN_SAMPLE_RATE;
	}else
	{
		if( tag->status_infect == STATUS_S){
			if( kontakt_infect==1) {
				time_kontakt = 0;
				kontakt_infect = 0;
			}else {
				if(time_kontakt < limit_timeout_contact_infect )
				{
					time_kontakt +=MAIN_SAMPLE_RATE;   //Increase timeout counter
				}
			}
			if( time_kontakt < limit_timeout_contact_infect )
			{
				timer_infect +=MAIN_SAMPLE_RATE;  //if contact is/was present increase infection timer
			}else {
				timer_infect = 0;
				reset_source();
			}
			if( timer_infect	> limit_time_infect ){
				status_change( STATUS_L,tag,time_counter);
				timer_infect = 0;
				time_kontakt = 0;
			}
		}
		if( tag->status_infect == STATUS_I)
		{
			if( kontakt_heal==1) {
				time_kontakt_heal = 0;
				kontakt_heal = 0;
			}else {
				if(time_kontakt_heal < limit_timeout_kontakt_heal )
				{
					time_kontakt_heal +=MAIN_SAMPLE_RATE;
				}
			}
			if( time_kontakt_heal < limit_timeout_kontakt_heal ) {
				timer_heal +=MAIN_SAMPLE_RATE;
			}else {
				timer_heal = 0;
			}
			if( timer_heal	> limit_time_heal )
			{
				status_change( STATUS_S,tag,time_counter);
				timer_infect = 0;
				time_kontakt = 0;
				timer_heal = 0;
			}
		}
	}
	if(tag->status_infect == STATUS_L)
	{
		if(timer_state > limit_time_latency )
		{
			status_change(STATUS_I,tag,time_counter);
		}
	}
	if(tag->status_infect == STATUS_I)
	{
		if(timer_state > limit_time_recovery  )
		{
			status_change(STATUS_R,tag,time_counter);
		}
		else if(timer_state > limit_time_suscept  )
		{
			status_change(STATUS_S,tag,time_counter);
		}
	}
}

#endif
