#include <zephyr/bluetooth/bluetooth.h>

struct  {
	int8_t 		rssi_threshold;	
} params_network;

typedef struct {
	uint8_t     id;
	int32_t    time;
	int8_t     rssi;
} contact_entry;

static uint16_t idx_read = 0;
static uint16_t idx_write = 0;

#define LENGTH_DATA_BUFFER 10

static contact_entry	data_array[LENGTH_DATA_BUFFER]; //ID 1 Byte; Ctr Start 4 Byte, RRSI 1 Byte


void network_init(void)
{
    params_network.rssi_threshold = -90; // RSSI threshold for contact evaluation
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

void network_evaluate_contact(const bt_addr_le_t *addr, 
    int8_t rssi, uint8_t adv_type, struct net_buf_simple *buf)
{
    struct net_buf_simple ad_temp;
    uint8_t id = 0;
    if (rssi >= params_network.rssi_threshold) 
    {
        
        net_buf_simple_clone(buf, &ad_temp);
	    bt_data_parse(&ad_temp, scan_extract_id, &id);
        data_array[idx_write] = (contact_entry){.id = id, .time = (int32_t)k_uptime_seconds(), .rssi = rssi};
        idx_write = (idx_write + 1) % LENGTH_DATA_BUFFER;
    }
}