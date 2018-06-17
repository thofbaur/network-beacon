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


#include "main.h"

APP_TIMER_DEF(m_main_timer_id);


static uint32_t led_status_mask = (1<<LED_RGB_GREEN);
static uint32_t time_counter = TIME_ZERO;  // changed after initial run
struct beacon tag ;

uint8_t time_sent = 0;

const uint8_t search_central[3] = CENTRAL_DEVICE_NAME;
const uint8_t search_beacon[3] = PERIPHERAL_DEVICE_NAME;


void set_status_led(uint8_t * p_show_status_led)
{
	uint32_t err_code;
	err_code = led_softblink_stop();
	APP_ERROR_CHECK(err_code);
	if( *p_show_status_led == 1)
	{
#ifdef SIMULATEINFECTION
		switch(tag.status_infect)
		{
		case STATUS_I:
			led_status_mask = (1<<LED_RGB_RED);
			break;
		case STATUS_V:
			led_status_mask = (1<<LED_RGB_BLUE) | (1<<LED_RGB_GREEN);
			break;
		case STATUS_R:
			led_status_mask = (1<<LED_RGB_BLUE);
			break;
		case STATUS_H:
			led_status_mask = (1<<LED_RGB_BLUE) | (1<<LED_RGB_RED);
			break;
		default:
			led_status_mask = (1<< LED_RGB_GREEN);
		}
#endif
	}else
	{
		led_status_mask = 1<<LED_RGB_GREEN;
	}
	err_code = led_softblink_start(led_status_mask);
	APP_ERROR_CHECK(err_code);
}

void update_beacon_info()
{
	uint8_t manuf_data[LENGTH_MANUF];

	network_update_tag();
#ifdef IDLIST
	manuf_data[0] = tag.id;
#endif
#ifdef SIMULATEINFECTION
	manuf_data[ADV_LENGTH_ID+1-1] = tag.status_infect | tag.inf_rev;
#endif
	manuf_data[LENGTH_MANUF-1] = tag.status_batt | tag.status_data;
	radio_update_adv(manuf_data);
}

#ifdef SIMULATEINFECTION
void update_tag_status_infect(uint8_t status_infect)
{
	tag.status_infect = status_infect;
}
#endif

void update_tag_inf_rev(uint8_t inf_rev)
{
	tag.inf_rev = (inf_rev)<<SHIFT_INF_REV;
}

void update_tag_status_data(uint8_t *p_status_data)
{
	tag.status_data = *p_status_data;
}



static void tag_init(void)
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

}



void evaluate_adv_report(const ble_gap_evt_t   * p_gap_evt)
{
	static uint8_t i;
	static uint8_t i_max;
	static uint8_t i_start;
	static uint8_t param;
	static uint32_t err_code;

	if(	p_gap_evt->params.adv_report.data[POS_NAME_START] == search_central[0] && \
    		p_gap_evt->params.adv_report.data[POS_NAME_START+1] == search_central[1] && \
			p_gap_evt->params.adv_report.data[POS_NAME_START+2] == search_central[2] )
    	{

			if( p_gap_evt->params.adv_report.data[POS_NAME_START+5]== 0xFF || p_gap_evt->params.adv_report.data[POS_NAME_START+5] == tag.id)
			{
				// Switch to control mode
				i_start = POS_NAME_START+5+1;
				i_max = p_gap_evt->params.adv_report.dlen;
//				for(i = POS_NAME_START+5+1;i<p_gap_evt->params.adv_report.dlen;i+=3)
				for(i = i_start;i<i_max;i+=3)
				{
					param = p_gap_evt->params.adv_report.data[i] & 0xE0;
					switch (param)
					{
						case P_BASE_MAIN:
						{
							//set a general parameter
							switch (p_gap_evt->params.adv_report.data[i] )
							{

								case P_BOOTLOADER:
								{
								   //  Write to the the GPREGRET REGISTER and reset the chip
								   //You can use the sd_-functions
									err_code = sd_power_gpregret_set(BOOTLOADER_DFU_START);
									APP_ERROR_CHECK(err_code);
									sd_nvic_SystemReset();
									break;
								}
								case P_SET_BEACON_MODE:
								{
									switch(p_gap_evt->params.adv_report.data[i+1])
									{
										case 1:
										{
											radio_control(P_SET_RAD_ACTIVE,1,0);
											network_control(P_TRACKING_ACTIVE,1,0);
											infect_control(P_SET_INF_ACTIVE,1,0,&tag,&time_counter);
											break;
										}
										case 0:
										{
											radio_control(P_SET_RAD_ACTIVE,0,0);
											network_control(P_TRACKING_ACTIVE,0,0);
											infect_control(P_SET_INF_ACTIVE,0,0,&tag,&time_counter);
											break;
										}
									break;
									}
								}
							}
							break;
						}
						#ifdef SIMULATEINFECTION
						case P_BASE_INF:
						{
							//set an infection parameter
							infect_control( (p_gap_evt->params.adv_report.data[i] ),p_gap_evt->params.adv_report.data[i+1], p_gap_evt->params.adv_report.data[i+2],&tag,&time_counter);
							break;
						}
						#endif
						case P_BASE_NETWORK:
						{
							//set a network parameter
							network_control((p_gap_evt->params.adv_report.data[i] ),p_gap_evt->params.adv_report.data[i+1], p_gap_evt->params.adv_report.data[i+2]);
							break;
						}
						case P_BASE_RADIO:
						{
							//set a radio parameter
							radio_control((p_gap_evt->params.adv_report.data[i] ),p_gap_evt->params.adv_report.data[i+1], p_gap_evt->params.adv_report.data[i+2]);
							break;
						}
						default:
						break;
					}
				}
			}
			infect_save_params();
			network_save_params();
			radio_save_params();
    	}
    	else if(	p_gap_evt->params.adv_report.data[POS_NAME_START] == search_beacon[0] && \
    		p_gap_evt->params.adv_report.data[POS_NAME_START+1] == search_beacon[1] && \
			p_gap_evt->params.adv_report.data[POS_NAME_START+2] == search_beacon[2] )
    	{
			#ifdef SIMULATEINFECTION
    		infect_evaluate_contact(&tag,p_gap_evt);
			#endif
    		network_evaluate_contact(p_gap_evt);
    	}
}

void main_reset_time_sent(void)
{
	time_sent = 0;
}



uint8_t main_nus_send_time(ble_nus_t * p_nus)
{

	uint8_t result_send=0;

    uint8_t data[4];

	while( !time_sent)
	{
		//General data
		data[2] = (time_counter      & 0xff) ;
		data[1] = (time_counter >>8  & 0xff) ;
		data[0] = (time_counter >>16 & 0xff) ;
#ifdef SIMULATEINFECTION
		data[3] =tag.status_infect | tag.status_batt | tag.inf_rev;
#else
		data[3] =tag.status_batt ;
#endif

		result_send = radio_nus_send(p_nus,data,4);
		if( result_send>0)
		{
			return time_sent;
		}
		else
		{
			time_sent = 1;
		}
	}
    return time_sent;
}

void sys_evt_dispatch(uint32_t sys_evt)
{
	fs_sys_event_handler(sys_evt);
	ble_advertising_on_sys_evt(sys_evt);

	if (sys_evt == NRF_EVT_POWER_FAILURE_WARNING)
	{
		sd_nvic_ClearPendingIRQ(SD_EVT_IRQn);
		sd_power_dcdc_mode_set(0);
		tag.status_batt = 0x10;
		update_beacon_info(&tag);
	}
}
#ifdef EXECUTEINRAM
__attribute__((used, long_call, section(".data"))) static void timer_main_event_handler(void* p_context)
#else
static void timer_main_event_handler(void* p_context)
#endif
{

	// Increment time since power on
	time_counter += MAIN_SAMPLE_RATE;
#ifdef SIMULATEINFECTION
	infect_main(&tag,&time_counter);
#endif
	network_main(&time_counter);
}

static void create_timers()
{
    uint32_t err_code;
    // Create timers
    err_code = app_timer_create(&m_main_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                timer_main_event_handler);
    APP_ERROR_CHECK(err_code);
}

static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

static void led_init(void)
{
	uint32_t err_code = NRF_SUCCESS;
	led_sb_init_params_t led_sb_init_param = LED_SB_INIT_DEFAULT_PARAMS(LEDS_MASK);

	led_sb_init_param.duty_cycle_max = 100;
	led_sb_init_param.off_time_ticks = 100000;// lowered for debug reasons: original value: 201072;
    err_code = led_softblink_init(&led_sb_init_param);
    APP_ERROR_CHECK(err_code);
}


// Simple event handler to handle errors during initialization.
static void fds_evt_handler(fds_evt_t const * const p_fds_evt)
{
    switch (p_fds_evt->id)
    {
        case FDS_EVT_INIT:
            if (p_fds_evt->result != FDS_SUCCESS)
            {
                // Initialization failed.
            }
            break;
        case FDS_EVT_GC:
        	break;
        default:
            break;
    }
}


bool main_record_exists(uint16_t file_id, uint16_t key)
{
	fds_record_desc_t   record_desc;
	ret_code_t ret;
	fds_find_token_t    ftok;
	memset(&ftok, 0x00, sizeof(fds_find_token_t));

	ret = fds_record_find(file_id, key, &record_desc, &ftok);
	if (ret == FDS_SUCCESS)
	{
		return true;
	}
	return false;
}

void main_save_data(void *p_data, uint8_t length, uint16_t file_id, uint16_t key)
{
	fds_record_t        record;
	fds_record_desc_t   record_desc;
	fds_record_chunk_t  record_chunk;
	ret_code_t ret;
	fds_find_token_t    ftok;


	memset(&ftok, 0x00, sizeof(fds_find_token_t));
	// Set up data.

	record_chunk.p_data         = p_data;
	record_chunk.length_words   = length;

	// Set up record.
	record.file_id                  = file_id;
	record.key               = key;
	record.data.p_chunks     = &record_chunk;
	record.data.num_chunks   = 1;


	ret = fds_record_find(file_id, key, &record_desc, &ftok);
	if (ret == FDS_SUCCESS)
	{
		ret = fds_record_update(&record_desc, &record);
	}
	else
	{
		ret = fds_record_write(&record_desc, &record);
	}
	if (ret ==FDS_ERR_NO_SPACE_IN_FLASH)
	{
		//execute garbage collection
		fds_gc();
	}
	else if (ret != FDS_SUCCESS)
	{
		// Handle error.
	}
}

bool main_read_data(void *p_data, uint8_t length, uint16_t file_id, uint16_t key)
{
	fds_flash_record_t  flash_record;
	fds_record_desc_t   record_desc;
	fds_find_token_t    ftok;
	bool	retval;

	retval = false;
	memset(&ftok, 0x00, sizeof(fds_find_token_t));
	// Loop until all records with the given key and file ID have been found.
	while (fds_record_find(file_id, key, &record_desc, &ftok) == FDS_SUCCESS)
	{
	    if (fds_record_open(&record_desc, &flash_record) != FDS_SUCCESS)
	    {
	        // Handle error. FDS_ERR_NOT_FOUND
	    }
	    // Access the record through the flash_record structure.

	    memcpy(p_data,flash_record.p_data,length);
	    retval = true;
	    // Close the record when done.
	    if (fds_record_close(&record_desc) != FDS_SUCCESS)
	    {
	        // Handle error.
	    }
	}
	return retval;
}

static void system_initialize(void)
{
	ret_code_t ret;

	ble_stack_init();
	led_init();
	ret = fds_register(fds_evt_handler);
	if (ret != FDS_SUCCESS)
	{
	    // Registering of the event handler has failed.
	}
	ret = fds_init();
	if (ret != FDS_SUCCESS)
	{
	    // Handle error.
	}
	network_init();
#ifdef SIMULATEINFECTION
	infect_init(&tag);
#endif
	tag_init();
	radio_params_init();

	fds_gc();
}


int main(void)
{
    uint32_t time_ms = MAIN_SAMPLE_RATE*1000; //Time(in milliseconds) for call of "main" loop.
    uint32_t err_code = NRF_SUCCESS;

    // Create application timers.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
    APP_ERROR_CHECK(err_code);
    create_timers();
    system_initialize();

    err_code = app_timer_start(m_main_timer_id, APP_TIMER_TICKS(time_ms, APP_TIMER_PRESCALER), NULL);
    APP_ERROR_CHECK(err_code);

	advertising_start();
	scan_start();
    err_code =  sd_power_dcdc_mode_set(1);
    APP_ERROR_CHECK(err_code);

	err_code = sd_power_pof_enable(1);
	APP_ERROR_CHECK(err_code);
	err_code = sd_power_pof_threshold_set(NRF_POWER_THRESHOLD_V23);
	APP_ERROR_CHECK(err_code);
    err_code = led_softblink_start(led_status_mask);
    APP_ERROR_CHECK(err_code);


    for (;; )
    {
    	power_manage();
    }
}
