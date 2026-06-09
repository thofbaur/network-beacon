#include <zephyr/bluetooth/bluetooth.h>
#include "radio.h"

struct  {
	int8_t 		rssi_threshold;	
} params_network;

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
static void contact_time_put(uint8_t time[3], uint32_t uptime_s)
{
	time[0] = (uptime_s >> 16) & 0xff;
	time[1] = (uptime_s >> 8) & 0xff;
	time[2] = uptime_s & 0xff;
}


void network_init(void)
{
    params_network.rssi_threshold = NETWORK_LIMIT_RSSI; // RSSI threshold for contact evaluation
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

void network_update_tag(void)
{
	static uint16_t number_dataset = 0;
	static uint8_t status_data = 0;

	if(idx_write >= idx_read)
	{
		number_dataset = idx_write-idx_read;
	}
	else
	{
		number_dataset = LENGTH_DATA_BUFFER-idx_read+idx_write;
	}
	if(number_dataset >DATA_LEVEL_7)
	{
		status_data = 7 << P_SHIFT_STATUS_DATA;
	}
	else if (number_dataset >DATA_LEVEL_6)
	{
		status_data = 6 << P_SHIFT_STATUS_DATA;
	}
	else if( number_dataset >DATA_LEVEL_5)
	{
		status_data = 5 << P_SHIFT_STATUS_DATA;
	}
	else if( number_dataset >DATA_LEVEL_4)
	{
		status_data = 4 << P_SHIFT_STATUS_DATA;
	}
	else if( number_dataset >DATA_LEVEL_3)
	{
		status_data = 3 << P_SHIFT_STATUS_DATA;
	}
	else if( number_dataset >DATA_LEVEL_2)
	{
		status_data = 2 << P_SHIFT_STATUS_DATA;
	}
	else if( number_dataset >DATA_LEVEL_1)
	{
		status_data = 1 << P_SHIFT_STATUS_DATA;
	}
	else
	{
		status_data = 0;
	}
	adv_update(ADV_POS_NETWORK_STATUS, status_data);
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
		
		data_array[idx_write].id = id;
		contact_time_put(data_array[idx_write].time, (uint32_t)k_uptime_seconds());
		data_array[idx_write].rssi = -rssi;
        idx_write = (idx_write + 1) % LENGTH_DATA_BUFFER;
        network_update_tag();
    }
}

uint8_t network_read_contact(uint8_t *buffer, uint16_t buffer_len)
{
	uint8_t bytes_written = 0;
	
	while (idx_read != idx_write)
	{
		if ((buffer_len - bytes_written) < CONTACT_ENTRY_SIZE) {
			break;
		}

		buffer[bytes_written++] = data_array[idx_read].id;
		buffer[bytes_written++] = data_array[idx_read].time[0];
		buffer[bytes_written++] = data_array[idx_read].time[1];
		buffer[bytes_written++] = data_array[idx_read].time[2];
		buffer[bytes_written++] = data_array[idx_read].rssi;

		idx_read = (idx_read + 1) % LENGTH_DATA_BUFFER;
	}

	if (bytes_written > 0) {
		network_update_tag();
	}
	
	return bytes_written;
}

