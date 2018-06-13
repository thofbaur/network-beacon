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
uint8_t infect_source[WIDTH_INFECT_ARRAY-4];
uint32_t infect_time;
uint8_t	inf_params_to_save;

static uint32_t timer_state = 0;

uint8_t idx_infect_read=0;
static uint8_t kontakt_infect = 0;
static uint8_t kontakt_heal = 0;

 struct {
	uint32_t limit_time_heal;
	uint32_t limit_time_latency;
	uint32_t limit_time_recovery;
	uint32_t limit_time_suscept;
	uint16_t limit_timeout_contact_infect; // 65 sekunden
	uint16_t limit_timeout_kontakt_heal; // 65 sekunden
	uint16_t	infect_revision;
	uint16_t limit_time_infect; //10 Minuten
	int8_t 	infect_limit_rssi;
	uint8_t	infect_status;
	uint8_t	infect_active;
} params_infect;

#define FILE_ID_INF     0x4001
#define REC_KEY_INF1    0x4001


void reset_source(void)
{
	memset(infect_source,0x00,WIDTH_INFECT_ARRAY-4);
}

void write_infect_array(struct beacon *p_tag)
{
	uint8_t i;
	static uint8_t idx_infect=0;

	infect_array[idx_infect][0] = p_tag->status_infect;
	infect_array[idx_infect][1] = infect_time >>16 & 0xff;
	infect_array[idx_infect][2] = infect_time >>8  & 0xff;
	infect_array[idx_infect][3] = infect_time >>0  & 0xff;
	for(i=0;i<WIDTH_INFECT_ARRAY-4;i++)
	{
		infect_array[idx_infect][4+i] = infect_source[i];
	}
	idx_infect +=1;
	if(idx_infect == LENGTH_INFECT_ARRAY)
	{
		idx_infect = 0;
	}
}

void infect_init(struct beacon *p_tag)
{

	bool ret;
	params_infect.limit_time_heal = LIMIT_HEAL;
	params_infect.limit_time_infect = LIMIT_INFECT;
	params_infect.limit_time_latency = LIMIT_LATENCY;
	params_infect.limit_time_recovery = LIMIT_RECOVERY;
	params_infect.limit_time_suscept = LIMIT_SUSCEPT;
	params_infect.limit_timeout_contact_infect = INFECT_TIMEOUT;
	params_infect.limit_timeout_kontakt_heal = HEAL_TIMEOUT;
	params_infect.infect_limit_rssi = INFECT_LIMIT_RSSI;
	params_infect.infect_status = INFECT_INITIAL_STATUS;
	params_infect.infect_active = INITIAL_MODE;
	ret =main_record_exists(FILE_ID_INF,REC_KEY_INF1);
	if(ret)
	{
		main_read_data(&params_infect,sizeof(params_infect),FILE_ID_INF,REC_KEY_INF1);
	}
	else
	{
		params_infect.infect_revision = INF_REV_INIT;
//

	}


	infect_time = TIME_ZERO;

	update_tag_status_infect(params_infect.infect_status);
	update_tag_inf_rev(params_infect.infect_revision);
	reset_source();
	write_infect_array(p_tag);
}

void infect_save_params(void)
{
//	static uint8_t data[28];

	if(inf_params_to_save ==1)
	{
//		memcpy(&data,&params_infect,sizeof(params_infect));
		main_save_data(&params_infect,sizeof(params_infect),FILE_ID_INF,REC_KEY_INF1);
		inf_params_to_save=0;
	}


}

void status_change(uint8_t status_new,struct beacon *p_tag, uint32_t *time_counter)
{
//	uint8_t i;
//    uint32_t       err_code;

	if(p_tag->status_infect == status_new)
	{
		//do nothing
	}else
	{
		infect_time = *time_counter;
		params_infect.infect_status = status_new;
		update_tag_status_infect(status_new);

		write_infect_array(p_tag);
		reset_source();
		set_status_led();
		timer_state = 0;
	}
	update_beacon_info();

}



void add_source(uint8_t tag_id)
{
	infect_source[(tag_id-1) >> 3] |=  (1<< ( 7-( (tag_id-1)&7) ) );
}

void infect_reset_idx_read(void)
{
	idx_infect_read =0;
}

uint8_t infect_nus_send_data(ble_nus_t * p_nus)
{
	uint8_t result_send =0;
	uint8_t infect_sent = 0;

	if (idx_infect_read == LENGTH_INFECT_ARRAY)
	{
		infect_sent = 1;
	}
    else
    {
    	infect_sent = 0;
    }
    while( (!infect_sent) )
    {

		result_send = radio_nus_send(p_nus, infect_array[idx_infect_read],WIDTH_INFECT_ARRAY*1);
		if(result_send>0)
		{
			infect_sent = 0;
			break;
		}
		else
		{
			idx_infect_read++;
			if (idx_infect_read == LENGTH_INFECT_ARRAY)
			{
				infect_sent = 1;
			}
		}
	}
  return infect_sent;
}

void infect_set_infect(uint8_t mode)
{
	params_infect.infect_active = mode;
}

void infect_evaluate_contact(struct beacon *p_tag,const ble_gap_evt_t   * p_gap_evt)
{
	if(params_infect.infect_active)
	{
		if( p_gap_evt->params.adv_report.rssi >= params_infect.infect_limit_rssi)
		{
			if(  p_tag->status_infect == STATUS_S && \
					(p_gap_evt->params.adv_report.data[POS_INF_STATUS] & INFECT_MASK) == STATUS_I && \
				(p_gap_evt->params.adv_report.data[POS_INF_STATUS] & INFECT_REV_MASK) == p_tag->inf_rev  )
			{
				kontakt_infect = 1;
				add_source(p_gap_evt->params.adv_report.data[POS_ID]);
			}
			if(  (p_gap_evt->params.adv_report.data[POS_INF_STATUS] & INFECT_MASK) == STATUS_H && \
				(p_gap_evt->params.adv_report.data[POS_INF_STATUS] & INFECT_REV_MASK) == p_tag->inf_rev && p_tag->status_infect == STATUS_I)
			{
				kontakt_heal = 1;
			}
		}
	}
}
void infect_main(struct beacon *p_tag,uint32_t *time_counter)
{
	static uint32_t time_kontakt = INFECT_TIMEOUT;
	static uint32_t time_kontakt_heal = HEAL_TIMEOUT;
	static uint32_t timer_infect = 0;
	static uint32_t timer_heal = 0;

	timer_state += MAIN_SAMPLE_RATE;



	if( p_tag->status_infect == STATUS_S){
		if( kontakt_infect==1) {
			time_kontakt = 0;
			kontakt_infect = 0;
		}else {
			if(time_kontakt < params_infect.limit_timeout_contact_infect )
			{
				time_kontakt +=MAIN_SAMPLE_RATE;   //Increase timeout counter
			}
		}
		if( time_kontakt < params_infect.limit_timeout_contact_infect )
		{
			timer_infect +=MAIN_SAMPLE_RATE;  //if contact is/was present increase infection timer
		}else {
			timer_infect = 0;
			reset_source();
		}
		if( timer_infect	> params_infect.limit_time_infect ){
			status_change( STATUS_L,p_tag,time_counter);
			timer_infect = 0;
			time_kontakt = 0;
		}
	}
	if( p_tag->status_infect == STATUS_I)
	{
		if( kontakt_heal==1) {
			time_kontakt_heal = 0;
			kontakt_heal = 0;
		}else {
			if(time_kontakt_heal < params_infect.limit_timeout_kontakt_heal )
			{
				time_kontakt_heal +=MAIN_SAMPLE_RATE;
			}
		}
		if( time_kontakt_heal < params_infect.limit_timeout_kontakt_heal ) {
			timer_heal +=MAIN_SAMPLE_RATE;
		}else {
			timer_heal = 0;
		}
		if( timer_heal	> params_infect.limit_time_heal )
		{
			status_change( STATUS_S,p_tag,time_counter);
			timer_infect = 0;
			time_kontakt = 0;
			timer_heal = 0;
		}
	}

	if(p_tag->status_infect == STATUS_L)
	{
		if(timer_state > params_infect.limit_time_latency )
		{
			status_change(STATUS_I,p_tag,time_counter);
		}
	}
	if(p_tag->status_infect == STATUS_I)
	{
		if(timer_state > params_infect.limit_time_recovery  )
		{
			status_change(STATUS_R,p_tag,time_counter);
		}
		else if(timer_state > params_infect.limit_time_suscept  )
		{
			status_change(STATUS_S,p_tag,time_counter);
		}
	}
}


void infect_control(uint8_t switch_param, uint8_t value1, uint8_t value2,struct beacon *p_tag,uint32_t *p_time_counter)
{
	switch (switch_param)
	{
		case P_RESET_INFECT:
		{
			if( value1==1)
			{
				kontakt_infect = 0;
				kontakt_heal = 0;
				reset_source();
				add_source(ID_ZENTRALE);
				status_change(INFECT_INITIAL_STATUS,p_tag,p_time_counter);
			}
			break;
		}
		case P_TIME_INFECT:
		{
			params_infect.limit_time_infect = (value1<<8) + value2 ;
			break;
		}
		case P_TIME_LATENCY:
		{
			params_infect.limit_time_latency = (value1<<(8+SHIFT_P_TIME_LATENCY)) + (value2<<SHIFT_P_TIME_LATENCY);
			break;
		}
		case P_TIME_RECOVER:
		{
			params_infect.limit_time_recovery =  (value1<<(8+SHIFT_P_TIME_RECOVER)) + (value2<<SHIFT_P_TIME_RECOVER);
			break;
		}
		case P_TIME_SUSCEPT:
		{
			params_infect.limit_time_suscept =  (value1<<(8+SHIFT_P_TIME_SUSCEPT)) + (value2<<SHIFT_P_TIME_SUSCEPT);
			break;
		}
		case P_TIMEOUT_INFECT:
		{
			params_infect.limit_timeout_contact_infect = (value1<<8) + value2;
			params_infect.limit_timeout_kontakt_heal =  params_infect.limit_timeout_contact_infect;
			break;
		}
		case P_RSSI_INFECT:
		{
			params_infect.infect_limit_rssi = value1 ;
			break;
		}
		case P_CHANGE_STATUS:
		{
			kontakt_infect = 0;
			kontakt_heal = 0;
			reset_source();
			add_source(ID_ZENTRALE);
			status_change(value1,p_tag,p_time_counter);
			break;
		}
		case P_INF_REV:
		{
			if (p_tag->inf_rev != ( value1<<SHIFT_INF_REV) )
			{
				params_infect.infect_revision = value1;
				update_tag_inf_rev(params_infect.infect_revision);

				kontakt_infect = 0;
				kontakt_heal = 0;
				reset_source();
				add_source(ID_ZENTRALE);
				status_change(INFECT_INITIAL_STATUS,p_tag,p_time_counter);
				inf_params_to_save=1;
				infect_save_params();

			}
			break;
		}
		default:
		break;
	}
}

#endif
