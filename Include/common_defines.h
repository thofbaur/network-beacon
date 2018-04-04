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


#ifndef COMMONDEFINES_H_
#define COMMINDEFINES_H_

/*
 * Compiler Switches to de-acitvate functionality
 */
//#define SIMULATEINFECTION
//#define IDLIST
#define RECORDNETWORK
//#define EXECUTEINRAM

#define LENGTH_DATA_BUFFER 401 //NO higher than 3700 otherwise Central readout will overflow



#define STATUS_S 					0
#define STATUS_L					1
#define STATUS_I					2
#define STATUS_R					3
#define STATUS_V					4
#define STATUS_VT					5
#define STATUS_H					6

//Identifier for all beacon, 3 bytes length
#define PERIPHERAL_DEVICE_NAME      	"AOM"
//Identifier for central administration beacon, 3 bytes length
#define CENTRAL_DEVICE_NAME      	"AOZ"


#ifdef IDLIST
	#define LENGTH_ID	1
#else
	#define LENGTH_ID	0
#endif
#ifdef SIMULATEINFECTION
	#define LENGTH_INF 	1
#else
#define LENGTH_INF 	0
#endif


#define LENGTH_MANUF 				 LENGTH_ID + LENGTH_INF+ 1
#define POS_NAME_START				2
#ifdef IDLIST
	#define POS_ID						7
#endif
#ifdef SIMULATEINFECTION
	#define POS_INF_STATUS					7+ LENGTH_ID
#endif
//#define POS_DATA					9// Used from enhanced version
#define SHIFT_INF_REV	5

#define MAX_NUM_TAGS		127
#define ID_ZENTRALE		127


#define INFECT_MASK	0x0F
#define INFECT_REV_MASK	0xF0

#define P_NULL			0
#define P_RESET_INFECT	1
#define P_TIME_INFECT	2
#define	P_TIME_LATENCY	3
#define	P_TIME_RECOVER	4
#define	P_TIME_SUSCEPT	5
#define P_TIMEOUT		6
#define P_RSSI			7
#define P_SHOW_STATUS	8
#define P_CHANGE_STATUS	9
#define P_TRACKING_ACTIVE 10
#define P_INF_REV		11

#ifdef SIMULATEINFECTION
#define LENGTH_INFECT_ARRAY	20
#define WIDTH_INFECT_ARRAY 19  // If this is changed, all infect_source lines have to be changed.
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
