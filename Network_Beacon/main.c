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


static uint8_t led_status = LED_RGB_GREEN;
static uint8_t show_status_led = INIT_SHOW_STATUS_LED;
static uint32_t time_counter = TIME_ZERO;  // changed after initial run
struct beacon tag ;

static uint8_t search_central[3] = CENTRAL_DEVICE_NAME;
static uint8_t search_beacon[3] = PERIPHERAL_DEVICE_NAME;


void set_status_led(void)
{
	uint32_t err_code;
	err_code = led_softblink_stop();
	APP_ERROR_CHECK(err_code);
	if( show_status_led == 1)
	{
#ifdef SIMULATEINFECTION
		switch(tag.status_infect)
		{
		case STATUS_I:
			led_status = LED_RGB_RED;
			break;
		case STATUS_V:
			led_status = LED_RGB_BLUE;
			break;
		case STATUS_R:
			led_status = LED_RGB_BLUE;
			break;
		default:
			led_status = LED_RGB_GREEN;
		}
#endif
	}else
	{
		led_status = LED_RGB_GREEN;
	}
	err_code = led_softblink_start((1<<led_status));
	APP_ERROR_CHECK(err_code);
}

#ifdef SIMULATEINFECTION
void update_tag_status_infect(uint8_t *status_infect)
{
	tag.status_infect = *status_infect;
}
#endif

void update_tag_inf_rev(uint8_t *inf_rev)
{
	tag.inf_rev = (*inf_rev)<<SHIFT_INF_REV;
}

void update_tag_status_data(uint8_t *status_data)
{
	tag.status_data = *status_data;
}

void update_beacon_info()
{
	uint8_t manuf_data[LENGTH_MANUF];

	network_update_tag();
#ifdef IDLIST
	manuf_data[0] = tag.id;
#endif
#ifdef SIMULATEINFECTION
	manuf_data[LENGTH_ID+1-1] = tag.status_infect | tag.inf_rev;
#endif
	manuf_data[LENGTH_MANUF-1] = tag.status_batt | tag.status_data;
	radio_update_adv(manuf_data);
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

	set_status_led();
}

void evaluate_adv_report(const ble_gap_evt_t   * p_gap_evt)
{
   	if(	p_gap_evt->params.adv_report.data[POS_NAME_START] == search_central[0] && \
    		p_gap_evt->params.adv_report.data[POS_NAME_START+1] == search_central[1] && \
			p_gap_evt->params.adv_report.data[POS_NAME_START+2] == search_central[2] )
    	{
//        		sender_id = p_gap_evt->params.adv_report.data[POS_ID];
//        		if( sender_id <=MAX_NUM_TAGS){
/*        			if( sender_id == search[3] ) // Admin Beacon
    			{
    				if( p_gap_evt->params.adv_report.data[POS_ID+1]== 0xFF || p_gap_evt->params.adv_report.data[POS_ID+1] == tag.id){
    					// Switch to control mode
    					for(i = POS_ID+2;i<32;i+=3)
    					{
    						switch (p_gap_evt->params.adv_report.data[i])
    						{
								case P_RESET_INFECT:
								{
	        						if( p_gap_evt->params.adv_report.data[i+1]==1)
									{
	        							kontakt_infect = 0;
	        							kontakt_heal = 0;
										reset_source();
										add_source(sender_id);
										status_change(INITIAL_STATUS,&tag,&time_counter);
										timer_reset = 0;
									}

									break;
								}
								case P_TIME_INFECT:
								{
									limit_time_infect = (p_gap_evt->params.adv_report.data[i+1]<<8) + p_gap_evt->params.adv_report.data[i+2] ;
									break;
								}
								case P_TIME_LATENCY:
								{
									limit_time_latency =  (p_gap_evt->params.adv_report.data[i+1]<<14) + (p_gap_evt->params.adv_report.data[i+2]<<6);
									break;
								}
								case P_TIME_RECOVER:
								{
									limit_time_recovery =  (p_gap_evt->params.adv_report.data[i+1]<<14) + (p_gap_evt->params.adv_report.data[i+2]<<6);
									break;
								}
								case P_TIME_SUSCEPT:
								{
									limit_time_suscept =  (p_gap_evt->params.adv_report.data[i+1]<<14) + (p_gap_evt->params.adv_report.data[i+2]<<6);
									break;
								}
								case P_TIMEOUT:
								{
									limit_timeout_contact_infect = (p_gap_evt->params.adv_report.data[i+1]<<8) + p_gap_evt->params.adv_report.data[i+2] ;
									limit_timeout_contact = limit_timeout_contact_infect;
									limit_timeout_kontakt_heal =  limit_timeout_contact_infect <<1;
									limit_netz		= 10+ limit_timeout_contact_infect;
									break;
								}
								case P_RSSI:
								{
									limit_rssi = - (p_gap_evt->params.adv_report.data[i+1]);
									break;
								}
								case P_SHOW_STATUS:
								{
									show_status_led = p_gap_evt->params.adv_report.data[i+1];
									set_status_led();
									break;
								}
								case P_CHANGE_STATUS:
								{
									reset_source();
									add_source(sender_id);
									status_change((p_gap_evt->params.adv_report.data[i+1]),&tag,&time_counter);
									break;
								}
								case P_TRACKING_ACTIVE:
								{
									tracking_active = (p_gap_evt->params.adv_report.data[i+1]);
									break;
								}
								case P_INF_REV:
								{
									if (tag.inf_rev != ( (p_gap_evt->params.adv_report.data[i+1])<<SHIFT_INF_REV) )
									{
										tag.inf_rev = (p_gap_evt->params.adv_report.data[i+1])<<SHIFT_INF_REV;
										kontakt_infect = 0;
										kontakt_heal = 0;
										reset_source();
										add_source(sender_id);
										status_change(INITIAL_STATUS,&tag,&time_counter);
									}
									break;
								}
								default:
									break;
    						}
    					}
    				}
    			}
    			else{*/
    	}
    	else if(	p_gap_evt->params.adv_report.data[POS_NAME_START] == search_beacon[0] && \
    		p_gap_evt->params.adv_report.data[POS_NAME_START+1] == search_beacon[1] && \
			p_gap_evt->params.adv_report.data[POS_NAME_START+2] == search_beacon[2] )
    	{
//
#ifdef SIMULATEINFECTION
    		infect_evaluate_contact(&tag,p_gap_evt);
#endif
    			network_evaluate_contact(p_gap_evt);
    	}
}

uint8_t main_nus_send_time(ble_nus_t * p_nus)
{
	uint8_t time_sent = 0;
	uint8_t result_send=0;
	uint32_t time_counter_sent=0;
    uint8_t data[4];

    if(time_counter_sent == time_counter)
    {
    	time_sent = 1;
    }
    else
    {
    	time_sent = 0;
    }
	while( !time_sent)
	{
		//General data
		data[3] = (time_counter      & 0xff) ;
		data[2] = (time_counter >>8  & 0xff) ;
		data[1] = (time_counter >>16 & 0xff) ;
#ifdef SIMULATEINFECTION
		data[0] =tag.status_infect | tag.status_batt | tag.inf_rev;
#else
		data[0] =tag.status_batt ;
#endif

		result_send = radio_nus_send(p_nus,data,4);
		if( result_send)
		{
			time_sent = 1;
			time_counter_sent = time_counter;
		}
	}
    return time_sent;
}

void sys_evt_dispatch(uint32_t sys_evt)
{
  //  ble_advertising_on_sys_evt(sys_evt);

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

//	uint32_t err_code;

//	static int16_t counter_scan = 0;
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

static void system_initialize(void)
{

	ble_stack_init();
	led_init();
	network_init();
#ifdef SIMULATEINFECTION
	infect_init(&tag);
#endif
	tag_init();
    gap_params_init();
    services_init();
    advertising_init();
    conn_params_init();
    scan_init();
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
    err_code = led_softblink_start((1<<led_status));
    APP_ERROR_CHECK(err_code);

    for (;; )
    {
    	power_manage();
    }
}
