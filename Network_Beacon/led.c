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

#include "led.h"

static uint32_t led_status_mask = (1<<LED_RGB_GREEN);



void led_init(void)
{
	uint32_t err_code = NRF_SUCCESS;
	led_sb_init_params_t led_sb_init_param = LED_SB_INIT_DEFAULT_PARAMS(LEDS_MASK);

	led_sb_init_param.duty_cycle_max = 100;
	led_sb_init_param.off_time_ticks = 100000;// lowered for debug reasons: original value: 201072;
    err_code = led_softblink_init(&led_sb_init_param);
    APP_ERROR_CHECK(err_code);
}

void led_start(void)
{
    uint32_t err_code = NRF_SUCCESS;

    err_code = led_softblink_start(led_status_mask);
    APP_ERROR_CHECK(err_code);
}
    



void set_status_led(uint8_t * p_show_status_led, struct beacon *p_tag)
{
	uint32_t err_code;
	err_code = led_softblink_stop();
	APP_ERROR_CHECK(err_code);
	if( *p_show_status_led == 1)
	{
#ifdef SIMULATEINFECTION
		switch(p_tag->status_infect)
		{
		case STATUS_I:
			led_status_mask = (1<<LED_RGB_RED);
			break;
		case STATUS_V:
			led_status_mask = (1<<LED_RGB_BLUE) | (1<<LED_RGB_GREEN);
			break;
		case STATUS_VT:
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