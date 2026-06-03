/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#include <zephyr/bluetooth/bluetooth.h>

#include "radio.h"
#include "network.h"

int main(void)
{
    int err;


	printk("Starting DSA Network Beacon\n");
	network_init();
	/* Initialize the Bluetooth Subsystem */
	err = radio_init();
	/* Start advertising and scanning*/
	err = radio_start();
	

	do {
		k_sleep(K_MSEC(400));
		radio_update();
		

		
	} while (1);
	return 0;
}
