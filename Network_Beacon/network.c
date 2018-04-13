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
 * network.c
 *
 *  Created on: 30.03.2018
 *      Author: thofbaur
 */

#include "network.h"

typedef struct network_param_struct {
	uint16_t limit_netz_flush;
	uint8_t limit_netz;
	uint16_t limit_timeout_contact;
	int8_t limit_rssi;
} network_param_struct;

network_param_struct params_network = {.limit_netz_flush = 1800, .limit_netz = NETWORK_CONTACTTIME, .limit_timeout_contact = NETWORK_TIMEOUT, .limit_rssi = NETWORK_LIMIT_RSSI};
static uint16_t idx_read = 0;
static uint16_t idx_write = 0;

uint8_t tracking_active = 1;
#ifdef IDLIST

static uint8_t  contact_tracker[MAX_NUM_TAGS-1][6];// Ctr 3 Byte;  Timeout 1 Byte, Contact Seen 1 Byte; Contact Active 1 Byte;
static uint8_t	data_array[LENGTH_DATA_BUFFER][NETWORK_SIZEDATA]; //ID 1 Byte; Ctr Start 3 Byte, Duration 2 Byte
#else

static uint8_t  contact_list[LENGTH_CONTACT_LIST][12]; //MAC 6 Byte, Ctr 3 Byte; Contact Seen 1 Byte; Contact Active 1 Byte; Timeout 1 Byte,
static uint8_t	data_array[LENGTH_DATA_BUFFER][NETWORK_SIZEDATA];//MAC 6 Byte, Start Ctr 3 Byte;Duration 2 Byte
#endif


void network_init()
{
	uint16_t i;

#ifdef IDLIST
	memset(&contact_tracker,0x00,(MAX_NUM_TAGS-1)*NETWORK_SIZEDATA);
	for (i=0;i<(MAX_NUM_TAGS-1);i++)
	{
		contact_tracker[i][5] = params_network.limit_timeout_contact;
	}
#else
	memset(&contact_list,0x00,LENGTH_CONTACT_LIST*12);
	for (i=0;i<LENGTH_CONTACT_LIST;i++)
	{
		contact_list[i][11] = limit_timeout_contact;
	}
#endif
}


void network_update_tag(void)
{
	static uint16_t number_data=0;
	static uint8_t status_data=0;

	if(idx_write >= idx_read)
	{
		number_data = idx_write-idx_read;
	}
	else
	{
		number_data = LENGTH_DATA_BUFFER-idx_read+idx_write;
	}
	if(number_data >DATA_LEVEL_7)
	{
		status_data = 7 << 5;
	}
	else if (number_data >DATA_LEVEL_6)
	{
		status_data = 6 << 5;
	}
	else if( number_data >DATA_LEVEL_5)
	{
		status_data = 5 << 5;
	}
	else if( number_data >DATA_LEVEL_4)
	{
		status_data = 4 << 5;
	}
	else if( number_data >DATA_LEVEL_3)
	{
		status_data = 3 << 5;
	}
	else if( number_data >DATA_LEVEL_2)
	{
		status_data = 2 << 5;
	}
	else if( number_data >DATA_LEVEL_1)
	{
		status_data = 1 << 5;
	}
	else
	{
		status_data = 0;
	}
	update_tag_status_data(&status_data);
}

uint8_t network_nus_send_data(ble_nus_t * p_nus)
{

	uint8_t data_sent = 0;
	uint8_t result_send = 0;
	uint16_t upper_lim;
    uint8_t	length;
    uint8_t i;
    uint8_t data[20];

	if (idx_read == idx_write)
	{
		data_sent = 1;
	}
	else
	{
		data_sent = 0;
	}

	while( !data_sent)
    {
    	if (idx_read<idx_write)
    	{
    		upper_lim = idx_write;
    	}
    	else
    	{
    		upper_lim  = LENGTH_DATA_BUFFER;
    	}

    	if (upper_lim-idx_read>= NETWORK_MAXLENGTH)
    	{
    		length = NETWORK_MAXLENGTH;
    	}
    	else
    	{
    		length  = upper_lim-idx_read;
    	}

    	for ( i=0;i<length*NETWORK_SIZEDATA;i++)
		{
			data[i] = *(data_array[idx_read] + i);
		}
    	if (length != 0)
    	{
    		result_send = radio_nus_send(p_nus,data,NETWORK_SIZEDATA*length);
    	}
    	if(result_send)
    	{
    		idx_read += length;
            update_beacon_info();
            if (idx_read >= LENGTH_DATA_BUFFER)
            {
            	idx_read = 0;
            }
        }
        if(idx_read == idx_write)
		{
			data_sent = 1;
			update_beacon_info();
		}
    }
	return data_sent;
}

void set_contact_active(const ble_gap_evt_t   * p_gap_evt)
{
#ifdef IDLIST
		static uint8_t sender_id =0;
					sender_id = p_gap_evt->params.adv_report.data[POS_ID];
					contact_tracker[sender_id-1][3]=1;
#else

	uint8_t i,j,k;
	uint8_t temp;
	for(i=0;i<LENGTH_CONTACT_LIST;i++)
	{
		// If contact already is in list, set contact to seen again
		if(memcmp(contact_list[i],p_gap_evt->params.connected.peer_addr.addr,6)==0)
		{
			contact_list[i][9]=1;
			break;
		}
	}
	// If contact is not in list, write to first unused line
	if(i == LENGTH_CONTACT_LIST)
	{
		for(j=0;j<LENGTH_CONTACT_LIST;j++)
		{
			// line is unused if contact is not active and contact was not seen
			if( (contact_list[j][10]==0) && (contact_list[j][6]==0))
			{
				memcpy(contact_list[j],p_gap_evt->params.connected.peer_addr.addr,6);
				contact_list[j][9]=1;
			}
		}
	}
	if(j==LENGTH_CONTACT_LIST)
	{
		temp= 0;
		for(k=1;k<LENGTH_CONTACT_LIST;k++)
				{
					// find oldest entry in contact list
					if( memcmp(&contact_list[temp][6],&contact_list[k][6],3)>0)
					{
						temp = k;
					}
				}
		// overwrite oldest entry with new entry
		//ToDo check this entry if to be written in data buffer
		memcpy(contact_list[temp],p_gap_evt->params.connected.peer_addr.addr,6);
		contact_list[temp][9]=1;
	}
#endif
}

void network_evaluate_contact(const ble_gap_evt_t   * p_gap_evt)
{
	if( p_gap_evt->params.adv_report.rssi >= params_network.limit_rssi)
	{
		if(tracking_active)
			{
				set_contact_active(p_gap_evt);
			}
	}
}

void write_contact_to_buffer(uint8_t *p_idx, uint16_t delta_contact)
{
#ifdef IDLIST
		data_array[idx_write][0] = *p_idx+1;
		memcpy(&data_array[idx_write][1],&contact_tracker[*p_idx],3);
		memcpy(&data_array[idx_write][4],&delta_contact,2);
#else
	memcpy(data_array[idx_write],&contact_list[*p_idx],9);
	memcpy(&data_array[idx_write][9],&delta_contact,2);
#endif

	idx_write++;
	if( idx_write >=  LENGTH_DATA_BUFFER)
	{
		idx_write = 0;
	}
	if( idx_write ==  idx_read) // if ring buffer is full, overwrite oldest entry
	{
		idx_read++;
	}
}

void network_main(uint32_t *p_time_counter)
{
	// Evaluate Network contacts
	// contact_tracker: time, time, time, see, act, to
#ifdef IDLIST
	static int16_t delta_contact = 0;
	static uint8_t i = 0;

	for (i=0;i<(MAX_NUM_TAGS-1);i++)
	{
		if( contact_tracker[i][3] ==1 ) //Contact was seen since last main cycle
		{
			contact_tracker[i][5] = 0;
			contact_tracker[i][3] = 0;
			if( contact_tracker[i][4] == 0) // contact entry not active
			{
				contact_tracker[i][4] = 1;
				contact_tracker[i][2] =  *p_time_counter     & 0xff;
				contact_tracker[i][1] =  *p_time_counter >>8 & 0xff ;
				contact_tracker[i][0] =  *p_time_counter >>16 & 0xff ;
			}
			else
			{
				delta_contact = ((uint32_t)contact_tracker[i][2]);
				delta_contact |=((uint32_t)contact_tracker[i][1])<<8;
				delta_contact |=((uint32_t)contact_tracker[i][0])<<16;
				delta_contact =  *p_time_counter - delta_contact;
				if(delta_contact > params_network.limit_netz_flush)
				{
					write_contact_to_buffer(&i,delta_contact & 0xffff);
					update_beacon_info();
					contact_tracker[i][2] =  *p_time_counter     & 0xff;
					contact_tracker[i][1] =  *p_time_counter >>8 & 0xff ;
					contact_tracker[i][0] =  *p_time_counter >>16 & 0xff ;
				}
			}
		}
		else
		{
			if( contact_tracker[i][4] == 1) // contact entry  active
			{
				if( contact_tracker[i][5] >= params_network.limit_timeout_contact) // check if timeout is reached
				{
					delta_contact = ((uint32_t)contact_tracker[i][2]);
					delta_contact |=((uint32_t)contact_tracker[i][1])<<8;
					delta_contact |=((uint32_t)contact_tracker[i][0])<<16;
					delta_contact =  *p_time_counter - delta_contact;
					if(delta_contact > params_network.limit_netz)
					{
//						write entry
						write_contact_to_buffer(&i,delta_contact & 0xffff);
						update_beacon_info();
					}
					contact_tracker[i][4] = 0;

				}
				else
				{
					contact_tracker[i][5] += MAIN_SAMPLE_RATE;
				}
			}
		}
	}
#else
		static int32_t delta_contact = 0;
	static uint8_t i = 0;

	for (i=0;i<LENGTH_CONTACT_LIST;i++)
		{
			if( contact_list[i][9] ==1 ) //Contact was seen since last main cycle
			{
				contact_list[i][9] = 0;
				contact_list[i][11] = 0;
				if( contact_list[i][10] == 0) // contact entry not active
				{
					contact_list[i][8] =  *p_time_counter     & 0xff;
					contact_list[i][7] =  *p_time_counter >>8 & 0xff ;
					contact_list[i][6]  =  *p_time_counter >>16 & 0xff ;
					contact_list[i][10]  = 1;
//					contact_list[i][11]  = 0;?
				}
				else
				{
					delta_contact = ((uint32_t)contact_list[i][8]);
					delta_contact |=((uint32_t)contact_list[i][7])<<8;
					delta_contact |=((uint32_t)contact_list[i][6])<<16;
					delta_contact =  *p_time_counter - delta_contact;
					if(delta_contact > limit_netz_flush)
					{
						write_contact_to_buffer(&i,delta_contact & 0xffff);
						update_beacon_info();
						contact_list[i][8] =  *p_time_counter       & 0xff;
						contact_list[i][7] =  *p_time_counter  >>8  & 0xff ;
						contact_list[i][6]  =  *p_time_counter >>16 & 0xff ;
					}
				}
			}
			else
			{
				if( contact_list[i][10] == 1) // contact entry  active
				{
					if( contact_list[i][11] >= limit_timeout_contact) // Check if timeout is reached
					{
						delta_contact = ((uint32_t)contact_list[i][8]);
						delta_contact |=((uint32_t)contact_list[i][7])<<8;
						delta_contact |=((uint32_t)contact_list[i][6])<<16;
						delta_contact =  *p_time_counter - delta_contact-limit_timeout_contact;
						if(delta_contact > limit_netz) // write entry if longer than threshold
						{
							write_contact_to_buffer(&i,delta_contact & 0xffff);
							update_beacon_info();
						}
						contact_list[i][10] = 0;
					}
					else
					{
						contact_list[i][11] += MAIN_SAMPLE_RATE;
					}
				}
			}
		}
#endif

}


void network_control(uint8_t switch_param, uint8_t value1, uint8_t value2)
{
	switch (switch_param)
	{

		case P_TIME_FLUSH:
		{
			params_network.limit_netz_flush = (value1<<8) + value2 ;
			break;
		}
		case P_TIME_NETWORK:
		{
			params_network.limit_netz = (value1<<(8)) + (value2<<0);
			break;
		}
		case P_TIMEOUT_NETWORK:
		{
			params_network.limit_timeout_contact = (value1<<8) + value2;

			break;
		}
		case P_RSSI_NETWORK:
		{
			params_network.limit_rssi = value1 ;
			break;
		}
		case P_TRACKING_ACTIVE:
		{
			tracking_active = (value1);
			break;
		}
		default:
		break;
	}
}

