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
 * interface_defs.h
 *
 *  Created on: 30.03.2018
 *      Author: thofbaur
 */

#ifndef INTERFACEDEFS_H_
#define INTERFACEDEFS_H_

#include "common_defines.h"

struct beacon {
	uint8_t id ;
	uint8_t status_infect;
	uint8_t status_batt;
	uint8_t status_data;
	uint8_t inf_rev;
};

#endif /* INTERFACEDEFS_H_ */
