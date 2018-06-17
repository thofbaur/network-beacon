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
 * radio.c
 *
 *  Created on: 30.03.2018
 *      Author: thofbaur
 */
#include "radio.h"

static ble_gap_adv_params_t m_adv_params;
static ble_gap_scan_params_t m_scan_param;
static uint8_t raw_advdata[7+LENGTH_MANUF];
static ble_nus_t                        m_nus;                                      /**< Structure to identify the Nordic UART Service. */
static uint16_t                         m_conn_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */
static uint16_t nus_cnt = 0;
static uint8_t nus_active = 0;

struct {
	uint16_t	adv_interval;
	uint16_t	adv_interval_passive;
	uint16_t	scan_interval;
	uint16_t	scan_interval_passive;
	uint16_t	scan_window;
	uint16_t	scan_window_passive;
	uint8_t		mode;
} params_radio;

bool	radio_params_to_save=0;
bool radio_params_hardcoded=0;
#define FILE_ID_RADIO     0x9001
#define REC_KEY_RADIO	    0x9001


void set_radio_params_init(void)
{
	params_radio.adv_interval 			= CONNECTABLE_ADV_INTERVAL;
	params_radio.adv_interval_passive 	= CONNECTABLE_ADV_INTERVAL_PASSIVE;
	params_radio.scan_interval 			= (uint16_t)SCAN_INTERVAL;
	params_radio.scan_interval_passive 	= (uint16_t)SCAN_INTERVAL_PASSIVE;
	params_radio.scan_window 			= (uint16_t)SCAN_WINDOW;
	params_radio.scan_window_passive 	= (uint16_t)SCAN_WINDOW_PASSIVE;
	params_radio.mode 					= INITIAL_MODE;
	radio_params_hardcoded = 1;
}

void radio_save_params(void)
{

	if(radio_params_to_save ==1)
	{
		main_save_data(&params_radio,sizeof(params_radio),FILE_ID_RADIO,REC_KEY_RADIO);
		radio_params_to_save=0;
	}
}

void advertising_start(void)
{
   uint32_t err_code;

   err_code = sd_ble_gap_adv_start(&m_adv_params);
   APP_ERROR_CHECK(err_code);
}

void scan_start(void)
{
   uint32_t err_code;
   //(void) sd_ble_gap_scan_stop();
   err_code = sd_ble_gap_scan_start(&m_scan_param);
   APP_ERROR_CHECK(err_code);
}

void set_ble_params(uint8_t mode)
{
	switch(mode)
			{
				case 0:
				{
				    m_adv_params.interval   = params_radio.adv_interval_passive;
				    m_scan_param.interval = params_radio.scan_interval_passive;
				    m_scan_param.window = params_radio.scan_window_passive;
					break;
				}
				case 1:
				{
					m_adv_params.interval   = params_radio.adv_interval;
					m_scan_param.interval = params_radio.scan_interval;
					m_scan_param.window = params_radio.scan_window;
				    break;
				}
			}
}


void scan_init(void)
{
    m_scan_param.active = 0;
    m_scan_param.selective = 0;
    m_scan_param.p_whitelist = NULL;
    m_scan_param.timeout   = SCAN_TIMEOUT;


    set_ble_params(params_radio.mode);
}

void advertising_init(void)
{
    uint32_t err_code;
    uint8_t i;
    const uint8_t local_name[3] =PERIPHERAL_DEVICE_NAME;

    raw_advdata[0] = 1+strlen(PERIPHERAL_DEVICE_NAME);
    raw_advdata[1] = BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME;
    for (i=0;i<3;i++)
    {
    	raw_advdata[2+i] =local_name[i];
    }
    raw_advdata[5] = 1+LENGTH_MANUF;
    raw_advdata[6] = BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA;
    err_code = sd_ble_gap_adv_data_set(raw_advdata,sizeof(raw_advdata),NULL,0);
    APP_ERROR_CHECK(err_code);

    update_beacon_info();

    // Initialize advertising parameters (used when starting advertising).
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.type       = BLE_GAP_ADV_TYPE_ADV_IND;
    m_adv_params.timeout    = 0;
//    m_adv_params.fp  		= BLE_GAP_ADV_FP_ANY;

    set_ble_params(params_radio.mode);
}

void radio_update_adv(uint8_t manuf_data[LENGTH_MANUF])
{
	uint32_t err_code;
	uint8_t i;
	for(i=0;i<LENGTH_MANUF;i++)
	{
		raw_advdata[7+i]= manuf_data[i];
	}
    err_code = sd_ble_gap_adv_data_set(raw_advdata,sizeof(raw_advdata),NULL,0);
    APP_ERROR_CHECK(err_code);
}




void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_sec_mode_t sec_mode;
    ble_gap_conn_params_t   gap_conn_params;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)PERIPHERAL_DEVICE_NAME, strlen(PERIPHERAL_DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_tx_power_set(TX_POWER);
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
   	gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

   	err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
   	APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling an event from the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module
 *          which are passed to the application.
 *
 * @note All this function does is to disconnect. This could have been done by simply setting
 *       the disconnect_on_fail config parameter, but instead we use the event handler
 *       mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
/*
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}
*/

/**@brief Function for handling errors from the Connection Parameters module.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = true; //false
    cp_init.evt_handler                    = NULL; //on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

#ifdef EXECUTEINRAM
__attribute__((used, long_call, section(".data"))) static uint8_t nus_push_data(ble_nus_t * p_nus)
#else
static uint8_t nus_push_data(ble_nus_t * p_nus)
#endif
{
	uint8_t all_sent = 0;
	uint8_t time_sent = 0;
	uint8_t data_sent = 0;
#ifdef SIMULATEINFECTION
	uint8_t infect_sent=0;
#endif

	all_sent = 0;
	time_sent = 0;
	data_sent = 0;
	infect_sent = 0;

	//Send current time counter
   time_sent = main_nus_send_time(p_nus);
   // Infection data
#ifdef SIMULATEINFECTION
   if(time_sent)
   {
	   infect_sent = infect_nus_send_data( p_nus);
   }

#endif
   //send network data
   if(infect_sent)
   {
	   data_sent = network_nus_send_data( p_nus);
   }

   all_sent = time_sent & data_sent;
#ifdef SIMULATEINFECTION
   all_sent &= infect_sent;
#endif

   return (all_sent);
}


/**@brief Function for handling the data from the Nordic UART Service.
*
* @details This function will process the data received from the Nordic UART BLE Service and send
*          it to the UART module.
*
* @param[in] p_nus    Nordic UART Service structure.
* @param[in] p_data   Data to be send to UART module.
* @param[in] length   Length of the data.
*/
/**@snippet [Handling the data received over BLE] */
void nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length)
{
	if (p_data[0]== 's' && p_data[1] == 't')
	{
   	nus_active = 1;
   	nus_cnt=0;
   	main_reset_time_sent();
#ifdef SIMULATEINFECTION
   	infect_reset_idx_read();
#endif
   	nus_push_data(p_nus);
	}
}

#ifdef EXECUTEINRAM
__attribute__((used, long_call, section(".data")))  static void on_ble_evt(ble_evt_t * p_ble_evt)
#else
static void on_ble_evt(ble_evt_t * p_ble_evt)
#endif
{
    const ble_gap_evt_t   * p_gap_evt = &p_ble_evt->evt.gap_evt;
//    static uint8_t sender_id =0;
    uint32_t err_code;
 //   uint8_t i;
    uint8_t final_msg[8] = { 'F','i','n','i','s','h','e','d'};
    uint8_t all_sent=0;
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_ADV_REPORT:
        {
        	evaluate_adv_report(p_gap_evt);
            break;
        }
        case BLE_GAP_EVT_CONNECTED:

            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break; // BLE_GAP_EVT_CONNECTED

        case BLE_GAP_EVT_DISCONNECTED:
            m_conn_handle = BLE_CONN_HANDLE_INVALID;

        	advertising_start();
            break; // BLE_GAP_EVT_DISCONNECTED

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported
        	 err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
        	    APP_ERROR_CHECK(err_code);
           break; // BLE_GAP_EVT_SEC_PARAMS_REQUEST

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_SYS_ATTR_MISSING

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTC_EVT_TIMEOUT

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_TIMEOUT

        case BLE_EVT_USER_MEM_REQUEST:
            err_code = sd_ble_user_mem_reply(p_ble_evt->evt.gattc_evt.conn_handle, NULL);
            APP_ERROR_CHECK(err_code);
            break; // BLE_EVT_USER_MEM_REQUEST
        case BLE_EVT_TX_COMPLETE:
        	nus_cnt--;
        	if(nus_active)
        	{
        		all_sent = nus_push_data(&m_nus);
        	}
        	if( all_sent)
        	{
        		if(nus_active)
        		{
        			err_code = ble_nus_string_send(&m_nus, final_msg, 8);
        			if (err_code == BLE_ERROR_NO_TX_PACKETS ||
							err_code == NRF_ERROR_INVALID_STATE ||
							err_code == BLE_ERROR_GATTS_SYS_ATTR_MISSING)
					{
						break;
					}
					else if (err_code != NRF_SUCCESS)
					{
						APP_ERROR_HANDLER(err_code);
					}
					else
					{
						nus_active = 0;
						nus_cnt++;
					}
        		}
        		else
        		{
        			if (nus_cnt == 0)
        			{
        				sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        			}
        		}
        	}
            break;
        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            ble_gatts_evt_rw_authorize_request_t  req;
            ble_gatts_rw_authorize_reply_params_t auth_reply;

            req = p_ble_evt->evt.gatts_evt.params.authorize_request;

            if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID)
            {
                if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ)     ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL))
                {
                    if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
                    }
                    else
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
                    }
                    auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
                    err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                               &auth_reply);
                    APP_ERROR_CHECK(err_code);
                }
            }
        } break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

#if (NRF_SD_BLE_API_VERSION == 3)
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                       NRF_BLE_MAX_MTU_SIZE);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST
#endif

        default:
            break;
    }
}

static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
	ble_conn_params_on_ble_evt(p_ble_evt);
	ble_nus_on_ble_evt(&m_nus, p_ble_evt);
    on_ble_evt(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
}

void ble_stack_init(void)
{
	nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;
    uint32_t err_code;
    ble_enable_params_t ble_enable_params;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    // Enable BLE stack
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
    		PERIPHERAL_LINK_COUNT,
			&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    //Check the ram settings against the used number of links
	CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT,PERIPHERAL_LINK_COUNT);

    // Enable BLE stack.
#if (NRF_SD_BLE_API_VERSION == 3)
    ble_enable_params.gatt_enable_params.att_mtu = NRF_BLE_MAX_MTU_SIZE;
#endif
	// Enable BLE stack.
	err_code = softdevice_enable(&ble_enable_params);
	APP_ERROR_CHECK(err_code);

	// Register with the SoftDevice handler module for BLE events.
	err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
	APP_ERROR_CHECK(err_code);

	// Register with the SoftDevice handler module for System events.
	err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing services that will be used by the application.
 */
void services_init(void)
{
    uint32_t       err_code;
    ble_nus_init_t nus_init;

    memset(&nus_init, 0, sizeof(nus_init));

    nus_init.data_handler = nus_data_handler;

    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);
}

uint8_t radio_nus_send(ble_nus_t * p_nus, uint8_t * p_string, uint16_t length)
{
    uint32_t       err_code = NRF_SUCCESS;
	err_code = ble_nus_string_send(p_nus, p_string, length);
	if (err_code == BLE_ERROR_NO_TX_PACKETS ||
						err_code == NRF_ERROR_INVALID_STATE ||
						err_code == BLE_ERROR_GATTS_SYS_ATTR_MISSING)
	{
		return 2;
	}
	else if (err_code != NRF_SUCCESS)
	{
		APP_ERROR_HANDLER(err_code);
		return 1;
	}
	else
	{
		nus_cnt++;
		return 0;
	}
}

void radio_params_init(void)
{
	bool ret;

		ret =main_read_data(&params_radio,sizeof(params_radio),FILE_ID_RADIO,REC_KEY_RADIO);
		if(ret)
		{
			// stored params have been loaded
		}
		else
		{
			// set hardcoded values
			set_radio_params_init();
			radio_params_to_save=1;
			radio_save_params();

		}
	gap_params_init();
	services_init();
	advertising_init();
	conn_params_init();
	scan_init();
}

void radio_control(uint8_t switch_param, uint8_t value1, uint8_t value2)
{
	switch (switch_param)
	{

		case P_ADV_INTERVAL:
		{
			if(  params_radio.adv_interval != MSEC_TO_UNITS(  (value1<<8) + value2  , UNIT_0_625_MS))
			{
			params_radio.adv_interval = MSEC_TO_UNITS(  (value1<<8) + value2  , UNIT_0_625_MS) ;
			radio_params_to_save=1;
			radio_params_hardcoded=0;
			}
			break;
		}
		case P_ADV_INTERVAL_PASSIVE:
		{
			if(params_radio.adv_interval_passive != MSEC_TO_UNITS(  (value1<<8) + value2  , UNIT_0_625_MS))
			{
			params_radio.adv_interval_passive =MSEC_TO_UNITS(  (value1<<8) + value2  , UNIT_0_625_MS) ;
			radio_params_to_save=1;
			radio_params_hardcoded=0;
			}
			break;
		}
		case P_SCAN_INTERVAL:
		{
			if(params_radio.scan_interval != MSEC_TO_UNITS(  (value1<<8) + value2  , UNIT_0_625_MS))
			{
			params_radio.scan_interval =MSEC_TO_UNITS(  (value1<<8) + value2  , UNIT_0_625_MS) ;
			radio_params_to_save=1;
			radio_params_hardcoded=0;
			}
			break;
		}
		case P_SCAN_INTERVAL_PASSIVE:
		{
			if(params_radio.scan_interval_passive != MSEC_TO_UNITS(  (value1<<8) + value2  , UNIT_0_625_MS))
			{
			params_radio.scan_interval_passive =MSEC_TO_UNITS(  (value1<<8) + value2  , UNIT_0_625_MS) ;
			radio_params_to_save=1;
			radio_params_hardcoded=0;
			}
			break;
		}
		case P_SCAN_WINDOW:
		{
			if(params_radio.scan_window !=MSEC_TO_UNITS(  (value1<<8) + value2  , UNIT_0_625_MS))
			{
			params_radio.scan_window = MSEC_TO_UNITS(  (value1<<8) + value2  , UNIT_0_625_MS);
			radio_params_to_save=1;
			radio_params_hardcoded=0;
			}
			break;
		}
		case P_SCAN_WINDOW_PASSIVE:
		{
			if(params_radio.scan_window_passive !=MSEC_TO_UNITS(  (value1<<8) + value2  , UNIT_0_625_MS))
			{
			params_radio.scan_window_passive = MSEC_TO_UNITS(  (value1<<8) + value2  , UNIT_0_625_MS) ;
			radio_params_to_save=1;
			radio_params_hardcoded=0;
			}
			break;
		}
		case P_RADIO_RESET_PARAMS:
		{
			if(!radio_params_hardcoded)
			{
				set_radio_params_init();
				radio_params_to_save=1;
				break;
			}
			break;
		}
		case P_SET_RAD_ACTIVE:
		{
			if(params_radio.mode != value1)
			{
				params_radio.mode = value1;
				radio_params_to_save=1;
				break;
			}
			break;
		}
		default:
		break;
	}
	if( radio_params_to_save)
	{
		uint32_t err_code;
		err_code =  sd_ble_gap_adv_stop();
		APP_ERROR_CHECK(err_code);
		err_code =  sd_ble_gap_scan_stop();
		APP_ERROR_CHECK(err_code);
		set_ble_params(params_radio.mode);

		advertising_start();
		scan_start();
	}
}

