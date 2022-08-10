/**
 *   Network_Control. Software to record the social network and simulate
 *   the spreading of an infection via BLE devices.
 *   Copyright (C) 2018  Tobias Hofbaur (tobias.hofbaur @ gmx.de)
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

//APP_TIMER_DEF(m_main_timer_id);

static ble_gap_adv_params_t m_adv_params;

/* Available control parameters
 *
 *
 * see common_defines.h
 *
 *
 *
 *
 */
// Set Target beacon
#define TARGET	0x2A    // 0xFF for all Beacons, or ID  (in Hex) for dedicated beacon


#define TIME_SUSCEPT	404000 //
#define TIME_RECOVER	360000
#define TIME_INFECT		420
#define TIME_HEAL		120
#define	TIME_EXPOSED_ALT	1  //1 sec
#define INF_REV			3

#define TIME_NETWORK	30
#define TIME_NETZ_FLUSH	240
#define NETZ_RSSI		80

#define R_ADV_INTERVAL	600
#define R_ADV_INTERVAL_PASSIVE	5000
#define R_SCAN_INTERVAL	1000
#define R_SCAN_INTERVAL_PASSIVE	10000
#define R_SCAN_WINDOW		300
#define R_SCAN_WINDOW_PASSIVE	200



static uint8_t raw_advdata[30] = {
		4, BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, 0x00,0x00,0x00,
		23, BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA,
		TARGET,
//		P_BOOTLOADER		, 1														, 0,
//		P_SET_BEACON_MODE	, 0														, 0,  // Set first param to 1 or 0

		P_TIME_INFECT		, ((TIME_INFECT >>8 ) & 0xFF)							, ((TIME_INFECT ) & 0xFF),
//		P_TIME_HEAL			, ((TIME_HEAL >>8 ) & 0xFF)							, ((TIME_HEAL ) & 0xFF),
		P_TIME_RECOVER		, ((TIME_RECOVER >>(8+SHIFT_P_TIME_RECOVER)) & 0xFF)	, ((TIME_RECOVER >> SHIFT_P_TIME_RECOVER) & 0xFF),
		P_TIME_SUSCEPT		, ((TIME_SUSCEPT >>(8+SHIFT_P_TIME_SUSCEPT)) & 0xFF)	, ((TIME_SUSCEPT >> SHIFT_P_TIME_SUSCEPT) & 0xFF),
//		P_TIME_EXPOSED_ALT		, ((TIME_EXPOSED_ALT >>(8+SHIFT_P_TIME_EXPOSED_ALT)) & 0xFF)	, ((TIME_EXPOSED_ALT >> SHIFT_P_TIME_EXPOSED_ALT) & 0xFF),
//		P_CHANGE_STATUS		, STATUS_I												, 0, // Set first param die desired status
//		P_SET_INF_ACTIVE	, 1														, 0, // Set first param to 1 or 0
//		P_INF_RESET_PARAMS	, 0														, 0,
		P_INF_REV			, INF_REV												, 0,
		P_SHOW_STATUS		, 1														, 0,  // Set first param to 1 or 0
//		P_RESET_INFECT		, 1														, 0,

//		P_TIME_FLUSH		, ((TIME_NETZ_FLUSH>>8) & 0xFF) 						, (TIME_NETZ_FLUSH & 0xFF),
//		P_TIME_NETWORK		, ((TIME_NETWORK>>8)&0xFF)								, (TIME_NETWORK & 0xFF),
//		P_NET_RESET_PARAMS	, 0														, 0,
//		P_TRACKING_ACTIVE	, 1														, 0, // Set first param to 1 or 0

//		P_ADV_INTERVAL		, ((R_ADV_INTERVAL  >>8)& 0xFF)							, ( R_ADV_INTERVAL & 0xFF),
//		P_ADV_INTERVAL_PASSIVE , (( R_ADV_INTERVAL_PASSIVE >>8)& 0xFF)				, (R_ADV_INTERVAL_PASSIVE  & 0xFF),
//		P_SCAN_INTERVAL		, (( R_SCAN_INTERVAL >>8)& 0xFF)							, (R_SCAN_INTERVAL  & 0xFF),
//		P_SCAN_INTERVAL_PASSIVE	, (( R_SCAN_INTERVAL_PASSIVE >>8)& 0xFF)				, (R_SCAN_INTERVAL_PASSIVE  & 0xFF),
//		P_SCAN_WINDOW		, (( R_SCAN_WINDOW >>8)& 0xFF)							, (  R_SCAN_WINDOW& 0xFF),
//		P_SCAN_WINDOW_PASSIVE	, ((R_SCAN_WINDOW_PASSIVE  >>8)& 0xFF)				, (R_SCAN_WINDOW_PASSIVE  & 0xFF),
//		P_RADIO_RESET_PARAMS	, 0													, 0,  // Set first param to 1 or 0

};


// Do not modify code below
static void gap_params_init(void)  //DONE
{
    uint32_t                err_code;
    ble_gap_conn_sec_mode_t sec_mode;

        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    
    err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)PERIPHERAL_DEVICE_NAME, strlen(PERIPHERAL_DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_tx_power_set(TX_POWER);
    APP_ERROR_CHECK(err_code);
}

static void advertising_init(void)
{
    uint32_t err_code;
    const uint8_t local_name[3] = CENTRAL_DEVICE_NAME;

    memcpy(&raw_advdata[2],local_name,3);

	err_code = sd_ble_gap_adv_data_set(raw_advdata,sizeof(raw_advdata),NULL,0);
    APP_ERROR_CHECK(err_code);
    // Initialize advertising parameters (used when starting advertising).
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.type       = BLE_GAP_ADV_TYPE_ADV_IND;
    m_adv_params.interval   = CONNECTABLE_ADV_INTERVAL;
    m_adv_params.timeout    = 0;
//    m_adv_params.fp  		= BLE_GAP_ADV_FP_ANY;
}


static void advertising_start(void)
{
    uint32_t err_code;

    err_code = sd_ble_gap_adv_start(&m_adv_params);
    APP_ERROR_CHECK(err_code);
}


static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

static void ble_stack_init(void)
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
//	err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
//	APP_ERROR_CHECK(err_code);
//
//	// Register with the SoftDevice handler module for System events.
//	err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
//	APP_ERROR_CHECK(err_code);
}

static void led_init(void)
{
	uint32_t err_code = NRF_SUCCESS;
	led_sb_init_params_t led_sb_init_param = LED_SB_INIT_DEFAULT_PARAMS(LEDS_MASK);

	led_sb_init_param.duty_cycle_max = 100;
	led_sb_init_param.off_time_ticks = 20000;// lowered for debug reasons: original value: 201072;
    err_code = led_softblink_init(&led_sb_init_param);
    APP_ERROR_CHECK(err_code);
}

int main(void)
{
    uint32_t err_code = NRF_SUCCESS;

    // Create application timers.
       APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
        APP_ERROR_CHECK(err_code);

	ble_stack_init();
	gap_params_init();
	led_init();
	advertising_init();
	advertising_start();

    err_code = led_softblink_start((LEDS_MASK));
    APP_ERROR_CHECK(err_code);

    for (;; )
    {
    	power_manage();
    }
}
