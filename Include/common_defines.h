/**
 *   Network-Beacon. Software to record the social network and simulate
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


#ifndef COMMONDEFINES_H_
#define COMMINDEFINES_H_

/*
 * Compiler Switches to de-acitvate functionality
 */
#define SIMULATEINFECTION  //Infection only works with ID List
#define RECORDNETWORK
#define IDLIST
#define EXECUTEINRAM

#define LENGTH_DATA_BUFFER 401 //NO higher than 3700 otherwise Central readout will overflow

#define STATUS_S 					0
#define STATUS_L					1
#define STATUS_I					2
#define STATUS_R					3
#define STATUS_V					4
#define STATUS_VT					5
#define STATUS_H					6


//Identifier for all beacon, 3 bytes length
#define PERIPHERAL_DEVICE_NAME      	"DSA"
//Identifier for central administration beacon, 3 bytes length
#define CENTRAL_DEVICE_NAME      	"DSZ"

/* Definition of Parameters to be used by Network Control
 *
 */
#define P_NULL				0
// Param_Base_Mask = 0xE0 -> Possible Values: 000 ... 111 << 5
#define P_BASE_MAIN			0x20
#define P_BASE_INF			0x40
#define P_BASE_NETWORK		0x60
#define P_BASE_RADIO		0x80


#define P_SET_BEACON_MODE	P_BASE_MAIN+1
#define P_BOOTLOADER		P_BASE_MAIN+2

// Defines for Infect Parameters should be in one block

#define P_RESET_INFECT		P_BASE_INF+1
#define P_TIME_INFECT		P_BASE_INF+2
#define P_TIME_HEAL			P_BASE_INF+11
#define SHIFT_P_TIME_INFECT		0
#define	P_TIME_LATENCY		P_BASE_INF+3
#define SHIFT_P_TIME_LATENCY	0
#define	P_TIME_RECOVER		P_BASE_INF+4
#define SHIFT_P_TIME_RECOVER	6
#define	P_TIME_SUSCEPT		P_BASE_INF+5
#define SHIFT_P_TIME_SUSCEPT	6
#define P_TIMEOUT_INFECT	P_BASE_INF+6
#define P_TIMEOUT_HEAL		P_BASE_INF+7
#define P_RSSI_INFECT		P_BASE_INF+8
#define P_CHANGE_STATUS		P_BASE_INF+9
#define P_INF_REV			P_BASE_INF+10
#define P_INF_RESET_PARAMS	P_BASE_INF+12
#define P_SET_INF_ACTIVE	P_BASE_INF+13
#define P_SHOW_STATUS		P_BASE_INF+14
// Defines for Network Parameters should be in one block

#define P_TIME_NETWORK		P_BASE_NETWORK+1
#define P_TIMEOUT_NETWORK	P_BASE_NETWORK+2
#define P_TIME_FLUSH		P_BASE_NETWORK+3
#define P_RSSI_NETWORK		P_BASE_NETWORK+4
#define P_TRACKING_ACTIVE 	P_BASE_NETWORK+13
#define P_NET_RESET_PARAMS 	P_BASE_NETWORK+12

// Defines for Radio Parameters should be in one block

#define P_ADV_INTERVAL			P_BASE_RADIO+1
#define P_ADV_INTERVAL_PASSIVE	P_BASE_RADIO+2
#define P_SCAN_INTERVAL			P_BASE_RADIO+3
#define P_SCAN_INTERVAL_PASSIVE	P_BASE_RADIO+4
#define P_SCAN_WINDOW			P_BASE_RADIO+5
#define P_SCAN_WINDOW_PASSIVE	P_BASE_RADIO+6
#define P_RADIO_RESET_PARAMS	P_BASE_RADIO+12
#define P_SET_RAD_ACTIVE		P_BASE_RADIO+13

#define MAX_NUM_TAGS		128
#define ID_ZENTRALE		128






#ifdef IDLIST
	#define ADV_LENGTH_ID	1
#else
	#define ADV_LENGTH_ID	0
#endif
#ifdef SIMULATEINFECTION
	#define LENGTH_INF 	1
#else
#define LENGTH_INF 	0
#endif

#define LENGTH_MANUF 				 ADV_LENGTH_ID + LENGTH_INF+ 1
#define POS_NAME_START				2
#ifdef IDLIST
	#define POS_ID						7
#endif
#ifdef SIMULATEINFECTION
	#define POS_INF_STATUS					7+ ADV_LENGTH_ID
#endif
#define SHIFT_INF_REV	5

#ifdef IDLIST
	#define NETWORK_MAXLENGTH 3
	#define NETWORK_SIZEDATA 6 // not allowed 5, 10
#else
	#define NETWORK_MAXLENGTH 1
	#define NETWORK_SIZEDATA 11
#endif

#define INFECT_MASK	0x0F
#define INFECT_REV_MASK	0xF0

#ifdef SIMULATEINFECTION
#define LENGTH_INFECT_ARRAY	25
#define WIDTH_INFECT_ARRAY 20  // If this is changed, all infect_source lines have to be changed.
#endif

#ifndef LED_RGB_RED
#define LED_RGB_RED		BSP_LED_2
#endif
#ifndef LED_RGB_GREEN
#define LED_RGB_GREEN	BSP_LED_0
#endif
#ifndef LED_RGB_BLUE
#define LED_RGB_BLUE	BSP_LED_1
#endif

#endif
