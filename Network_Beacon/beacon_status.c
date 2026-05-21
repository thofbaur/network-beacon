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


#include "beacon_status.h"

struct beacon tag ;

struct beacon * tag_init(void)
{
	uint32_t                err_code;
	ble_gap_addr_t tag_adress;

	err_code = sd_ble_gap_address_get(&tag_adress);
	APP_ERROR_CHECK(err_code);
	tag.id = 0;
	tag.status_batt = 0;
#ifdef IDLIST
	uint16_t i;
	uint8_t j;
	uint8_t fail;

	for ( i = 0; i<NUM_MACS;i++)
	{
		fail = 0;
		for ( j = 0; j<6 && fail ==0;j++)
		{

			if ( (tag_adress.addr[j]) != (list_macs[i][j]))
			{
				fail = 1;

				break;
			}
		}
		if (fail == 0)
		{
			tag.id = i+1;
			break;
		}
	}
#endif
    return &tag;
}


void update_beacon_info()
{
	uint8_t manuf_data[LENGTH_MANUF];

	network_update_tag(tag);
#ifdef IDLIST
	manuf_data[0] = tag.id;
#endif
#ifdef SIMULATEINFECTION
	manuf_data[ADV_LENGTH_ID+1-1] = tag.status_infect | tag.inf_rev;
#endif
	manuf_data[LENGTH_MANUF-1] = tag.status_batt | tag.status_data;
	radio_update_adv(manuf_data);
}

uint8_t get_tag_id(void)
{
    return tag.id;
}

void set_tag_status_batt(uint8_t status_batt)
{
    tag.status_batt = status_batt;
    update_beacon_info();
}

uint8_t get_tag_status_batt(void)
{
    return tag.status_batt;
}



#ifdef SIMULATEINFECTION
void set_tag_status_infect(uint8_t status_infect)
{
	tag.status_infect = status_infect;
    update_beacon_info();
}
uint8_t get_tag_status_infect(void)
{
	return tag.status_infect ;
}

#endif

void set_tag_inf_rev(uint8_t inf_rev)
{
	tag.inf_rev = (inf_rev)<<SHIFT_INF_REV;
    update_beacon_info();
}

uint8_t get_tag_inf_rev(void)
{
    return tag.inf_rev ;
}

void set_tag_status_data(uint8_t *p_status_data)
{
	tag.status_data = *p_status_data;
}