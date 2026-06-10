#include <errno.h>
#include <string.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "defines.h"
#include "device.h"
#include "network.h"
#include "param_storage.h"
#include "radio.h"

#define NETWORK_PARAMS_STORAGE_KEY "dsa/network"

struct network_params {
	int8_t 		rssi_threshold;	
};

static struct network_params params_network;

typedef struct {
	uint8_t     id;
	uint8_t     time[3];
	uint8_t     rssi;  // negative value, e.g., -80 dBm is stored as 80
} contact_entry;
#define CONTACT_ENTRY_SIZE 5





#define LENGTH_DATA_BUFFER 10

#define NETWORK_LIMIT_RSSI		-80 // approx. 1-2m distance

#define DATA_LEVEL_1	0
#define DATA_LEVEL_2	1
#define DATA_LEVEL_3	2   // TODO 10
#define DATA_LEVEL_4	3   // TODO 100
#define DATA_LEVEL_5	500
#define DATA_LEVEL_6	1000
#define DATA_LEVEL_7	2000

#define P_SHIFT_STATUS_DATA 5
static contact_entry	data_array[LENGTH_DATA_BUFFER]; // ID 1 Byte; time 3 Byte; RSSI 1 Byte
static uint16_t idx_read = 0;
static uint16_t idx_write = 0;
static uint16_t contact_count = 0;
static K_MUTEX_DEFINE(contact_lock);

static void contact_time_put(uint8_t time[3], uint32_t uptime_s)
{
	time[0] = (uptime_s >> 16) & 0xff;
	time[1] = (uptime_s >> 8) & 0xff;
	time[2] = uptime_s & 0xff;
}

static void reset_parameters(void)
{
	params_network.rssi_threshold = NETWORK_LIMIT_RSSI;

}


void network_init(void)
{
	int err;

	reset_parameters();
	err = network_params_load();
	if (err == -ENOENT) {
		printk("No stored network parameters, using defaults\n");
	} else if (err) {
		printk("Failed to load network parameters (err %d), using defaults\n", err);
	}
}

void network_apply_command(uint8_t parameter, uint16_t value)
{
	struct network_params old_params_network = params_network;

	switch (parameter) {
	case P_RSSI_NETWORK:
		params_network.rssi_threshold = -(int8_t)value;
		printk("Network RSSI threshold set to %d dBm\n", params_network.rssi_threshold);
		break;
	case P_NETWORK_RESET_PARAMS:
		reset_parameters();
		printk("Network parameters reset\n");
		break;
	case P_TRACKING_ACTIVE:
		printk("Network tracking command value %u not implemented\n", value);
		break;
	default:
		printk("Unknown network parameter 0x%02x value %u\n", parameter, value);
		break;
	}

	if (memcmp(&old_params_network, &params_network, sizeof(params_network)) != 0) {
		int err = network_params_save();

		if (err) {
			printk("Failed to save network parameters (err %d)\n", err);
		}
	}
}

int network_params_load(void)
{
	return param_storage_load(NETWORK_PARAMS_STORAGE_KEY,
				  &params_network, sizeof(params_network));
}

int network_params_save(void)
{
	return param_storage_save(NETWORK_PARAMS_STORAGE_KEY,
				  &params_network, sizeof(params_network));
}

static bool scan_extract_id(struct bt_data *data, void *user_data)
{
    uint8_t *id = user_data;
    
    if (data->type == BT_DATA_MANUFACTURER_DATA)
    {
        data->data_len >= 1 ? (*id = data->data[0]) : (*id = 0);
        
        return false;
    }

    return true;
}

static uint8_t contact_status_from_count(uint16_t number_dataset)
{
	if (number_dataset > DATA_LEVEL_7) {
		return 7 << P_SHIFT_STATUS_DATA;
	}
	if (number_dataset > DATA_LEVEL_6) {
		return 6 << P_SHIFT_STATUS_DATA;
	}
	if (number_dataset > DATA_LEVEL_5) {
		return 5 << P_SHIFT_STATUS_DATA;
	}
	if (number_dataset > DATA_LEVEL_4) {
		return 4 << P_SHIFT_STATUS_DATA;
	}
	if (number_dataset > DATA_LEVEL_3) {
		return 3 << P_SHIFT_STATUS_DATA;
	}
	if (number_dataset > DATA_LEVEL_2) {
		return 2 << P_SHIFT_STATUS_DATA;
	}
	if (number_dataset > DATA_LEVEL_1) {
		return 1 << P_SHIFT_STATUS_DATA;
	}

	return 0;
}

void network_update_tag(void)
{
	int err;
	uint16_t number_dataset;

	k_mutex_lock(&contact_lock, K_FOREVER);
	number_dataset = contact_count;
	k_mutex_unlock(&contact_lock);

	device_set_network_status(contact_status_from_count(number_dataset));
	err = adv_update();
	if (err) {
		printk("Failed to update network status advertising data (err %d)\n", err);
	}
}

void network_evaluate_contact(const bt_addr_le_t *addr, 
    int8_t rssi, uint8_t adv_type, struct net_buf_simple *buf)
{
    struct net_buf_simple ad_temp;
    uint8_t id = 0;
    if (rssi >= params_network.rssi_threshold) 
    {
        
        net_buf_simple_clone(buf, &ad_temp);
	    bt_data_parse(&ad_temp, scan_extract_id, &id);
		
		k_mutex_lock(&contact_lock, K_FOREVER);
		data_array[idx_write].id = id;
		contact_time_put(data_array[idx_write].time, (uint32_t)k_uptime_seconds());
		data_array[idx_write].rssi = -rssi;
		idx_write = (idx_write + 1) % LENGTH_DATA_BUFFER;

		if (contact_count == LENGTH_DATA_BUFFER) {
			idx_read = (idx_read + 1) % LENGTH_DATA_BUFFER;
		} else {
			contact_count++;
		}
		k_mutex_unlock(&contact_lock);

		network_update_tag();
    }
}

uint8_t network_peek_contact(uint8_t *buffer, uint16_t buffer_len)
{
	uint8_t bytes_written = 0;
	uint16_t read_idx;
	uint16_t entries_left;
	
	k_mutex_lock(&contact_lock, K_FOREVER);
	read_idx = idx_read;
	entries_left = contact_count;

	while (entries_left > 0)
	{
		if ((buffer_len - bytes_written) < CONTACT_ENTRY_SIZE) {
			break;
		}

		buffer[bytes_written++] = data_array[read_idx].id;
		buffer[bytes_written++] = data_array[read_idx].time[0];
		buffer[bytes_written++] = data_array[read_idx].time[1];
		buffer[bytes_written++] = data_array[read_idx].time[2];
		buffer[bytes_written++] = data_array[read_idx].rssi;

		read_idx = (read_idx + 1) % LENGTH_DATA_BUFFER;
		entries_left--;
	}

	k_mutex_unlock(&contact_lock);

	return bytes_written;
}

void network_commit_contact_read(uint8_t bytes_read)
{
	uint16_t entries_read = bytes_read / CONTACT_ENTRY_SIZE;

	if (entries_read == 0) {
		return;
	}

	k_mutex_lock(&contact_lock, K_FOREVER);
	while (entries_read > 0 && contact_count > 0) {
		idx_read = (idx_read + 1) % LENGTH_DATA_BUFFER;
		contact_count--;
		entries_read--;
	}
	k_mutex_unlock(&contact_lock);

	network_update_tag();
}
