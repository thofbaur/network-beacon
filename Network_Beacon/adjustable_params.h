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
#ifndef ADJUSTABLEPARAMS_H
#define ADJUSTABLEPARAMS_H


/* Main Parameters
 *
 */

#define INITIAL_MODE				1  //1 Active, 0 Passive
#define MAIN_SAMPLE_RATE			8  // sample rate for main loop in seconds
#define TIME_ZERO					0x0000   // Time initialization

/* Infection Paramters
 */
#define INFECT_INITIAL_STATUS				STATUS_S

#define INFECT_TIMEOUT			65 // Timeout for received adv. packet in seconds. 0..255 before infect contact status is reset. Compensate for "lost" pakets.
#define HEAL_TIMEOUT			120 // Timeout for received adv. packet in seconds. 0..255 before heal contact status is reset. Compensate for "lost" pakets.
#define LIMIT_INFECT 				300 // Time needed with established contact for infection in seconds
#define LIMIT_RECOVERY				18000 // Time to Recovery in seconds //    6h (SIR-Modell)
#define LIMIT_LATENCY				5400 //  in seconds 1500  / 2h
#define LIMIT_HEAL					300 // Time to heal in seconds  300
#define LIMIT_SUSCEPT				1400000 // Time to suscept in seconds  (SIS-Modell)
#define INFECT_LIMIT_RSSI		 	-80 // approx. 1-2m distance
#define INIT_SHOW_STATUS_LED		0  // Determines whether the LED shall represent the current infect status
#define INF_REV_INIT 				0

/* Radio Parameters
 *
 */
#define ADV_INTERVAL				100 // Advertisement interval in milliseconds
#define SCAN_WINDOW_MS				120  //scan window in milliseconds
#define SCAN_INTERVAL_MS			10000  // scan interval in milliseconds
#define ADV_INTERVAL_PASSIVE				10000 // Advertisement interval in milliseconds
#define SCAN_WINDOW_MS_PASSIVE				200  //scan window in milliseconds
#define SCAN_INTERVAL_MS_PASSIVE			10000  // scan interval in milliseconds

/* Network Parameters
 * *
 */

#define NETWORK_LIMIT_RSSI		-80 // approx. 1-2m distance
#define DATA_LEVEL_1	0
#define DATA_LEVEL_2	1
#define DATA_LEVEL_3	10
#define DATA_LEVEL_4	100
#define DATA_LEVEL_5	200
#define DATA_LEVEL_6	300
#define DATA_LEVEL_7	500

#define NETWORK_TIMEOUT			65 // Timeout for received adv. packet in seconds. 0..255 before infect contact status is reset. Compensate for "lost" pakets.
#define NETWORK_CONTACTTIME		120

#define LENGTH_CONTACT_LIST 400  //ToDo


#endif
