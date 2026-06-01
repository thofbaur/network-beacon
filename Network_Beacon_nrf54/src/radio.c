#include <zephyr/bluetooth/bluetooth.h>
#include "radio_ids.h"
/* Radio Parameters
 *
 */
#define ADV_INTERVAL_MIN				90 // Advertisement interval in milliseconds
#define ADV_INTERVAL_MAX				120 // Advertisement interval in milliseconds
#define SCAN_WINDOW_MS				120  //scan window in milliseconds
#define SCAN_INTERVAL_MS			10000  // scan interval in milliseconds
// Low activity parameters
#define ADV_INTERVAL_MS_MIN_LOW_ACTIVITY				10000 // Advertisement interval in milliseconds
#define ADV_INTERVAL_MS_MAX_LOW_ACTIVITY				10100 // Advertisement interval in milliseconds
#define SCAN_WINDOW_MS_LOW_ACTIVITY				100  //scan window in milliseconds
#define SCAN_INTERVAL_MS_LOW_ACTIVITY			10000  // scan interval in milliseconds




#define CONNECTABLE_ADV_INTERVAL_MIN    		BT_GAP_MS_TO_ADV_INTERVAL(ADV_INTERVAL_MIN)
#define CONNECTABLE_ADV_INTERVAL_MAX    		BT_GAP_MS_TO_ADV_INTERVAL(ADV_INTERVAL_MAX)
#define SCAN_WINDOW								BT_GAP_MS_TO_SCAN_WINDOW(SCAN_WINDOW_MS)  //scan window in 0.625 mus
#define SCAN_INTERVAL							BT_GAP_MS_TO_SCAN_INTERVAL(SCAN_INTERVAL_MS)  // scan interval in 0.625 mus
#define CONNECTABLE_ADV_INTERVAL_MIN_LOW_ACTIVITY    BT_GAP_MS_TO_ADV_INTERVAL(ADV_INTERVAL_MS_MIN_LOW_ACTIVITY)
#define CONNECTABLE_ADV_INTERVAL_MAX_LOW_ACTIVITY    BT_GAP_MS_TO_ADV_INTERVAL(ADV_INTERVAL_MS_MAX_LOW_ACTIVITY)
#define SCAN_WINDOW_LOW_ACTIVITY					BT_GAP_MS_TO_SCAN_WINDOW(SCAN_WINDOW_MS_LOW_ACTIVITY)  //scan window in 0.625 mus
#define SCAN_INTERVAL_LOW_ACTIVITY				BT_GAP_MS_TO_SCAN_INTERVAL(SCAN_INTERVAL_MS_LOW_ACTIVITY)  // scan interval in 0.625 mus
#define HIGH_ACTIVITY				1
#define LOW_ACTIVITY				0
#define INITIAL_MODE				HIGH_ACTIVITY  //1 High Activity, 0 Low Activity


static uint8_t mfg_data[] = { 0xff, 0xff, 0x00 };

static const struct bt_data ad[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, strlen(CONFIG_BT_DEVICE_NAME)),
	BT_DATA(BT_DATA_MANUFACTURER_DATA, mfg_data, sizeof(mfg_data)),
};

struct {
	uint16_t	adv_interval_min;
	uint16_t	adv_interval_max;
	uint16_t	adv_interval_min_lowactivity;
	uint16_t	adv_interval_max_lowactivity;
	uint16_t	scan_interval;
	uint16_t	scan_interval_lowactivity;
	uint16_t	scan_window;
	uint16_t	scan_window_lowactivity;
	uint8_t		mode;
} params_radio;

struct bt_le_scan_param scan_param;
struct bt_le_adv_param adv_params;

static uint8_t device_id;

bool radio_params_hardcoded=0;



static uint8_t lookup_device_id(const bt_addr_t *addr)
{
    for (size_t i = 0; i < ARRAY_SIZE(device_id_table); i++) {
        if (bt_addr_cmp(addr, &device_id_table[i].addr) == 0) {
            return device_id_table[i].id;
        }
    }

    return 0xff; // unknown / unassigned
}

uint8_t get_device_id()
{
    bt_addr_le_t addrs[CONFIG_BT_ID_MAX];
    size_t count = CONFIG_BT_ID_MAX;

    // Retrieve all addresses registered for the Bluetooth stack
    bt_id_get(addrs, &count);
	device_id = lookup_device_id(&addrs[0].a);

    if (count > 0) {
        char addr_str[BT_ADDR_LE_STR_LEN];
        bt_addr_le_to_str(&addrs[0], addr_str, sizeof(addr_str));
        printk("Device Address: %s\n", addr_str);
    
	
		printk("Raw bytes: %02x %02x %02x %02x %02x %02x\n",
       	addrs[0].a.val[0], addrs[0].a.val[1], addrs[0].a.val[2],
       	addrs[0].a.val[3], addrs[0].a.val[4], addrs[0].a.val[5]);
	}
	return device_id;
}

void set_radio_params_init(void)
{
	params_radio.adv_interval_min 			= CONNECTABLE_ADV_INTERVAL_MIN;
	params_radio.adv_interval_max 			= CONNECTABLE_ADV_INTERVAL_MAX;
	params_radio.adv_interval_min_lowactivity 	= CONNECTABLE_ADV_INTERVAL_MIN_LOW_ACTIVITY;
	params_radio.adv_interval_max_lowactivity 	= CONNECTABLE_ADV_INTERVAL_MAX_LOW_ACTIVITY;
	params_radio.scan_interval 			= (uint16_t)SCAN_INTERVAL;
	params_radio.scan_interval_lowactivity 	= (uint16_t)SCAN_INTERVAL_LOW_ACTIVITY;
	params_radio.scan_window 			= (uint16_t)SCAN_WINDOW;
	params_radio.scan_window_lowactivity 	= (uint16_t)SCAN_WINDOW_LOW_ACTIVITY;
	params_radio.mode 					= INITIAL_MODE;
	radio_params_hardcoded = 1;
}

static void scan_cb(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type,
		    struct net_buf_simple *buf)
{
	mfg_data[2]++;
}

void scan_init(void)
{
    scan_param.type = BT_LE_SCAN_TYPE_PASSIVE;
	scan_param.options = BT_LE_SCAN_OPT_NONE;
}

void adv_init(void)
{
	adv_params.id = 0U;
	adv_params.sid = 0U;
	adv_params.secondary_max_skip = 0U;
	adv_params.options = BT_LE_ADV_NCONN_IDENTITY;
	mfg_data[0] = get_device_id();
}

void set_ble_params(uint8_t mode)
{
	switch(mode)
			{
				case LOW_ACTIVITY:
				{
				    adv_params.interval_min   = params_radio.adv_interval_min_lowactivity;
					adv_params.interval_max   = params_radio.adv_interval_max_lowactivity;
				    scan_param.interval = params_radio.scan_interval_lowactivity;
				    scan_param.window = params_radio.scan_window_lowactivity;
					break;
				}
				case HIGH_ACTIVITY:
				{
					adv_params.interval_min   = params_radio.adv_interval_min;
					adv_params.interval_max   = params_radio.adv_interval_max;
					scan_param.interval = params_radio.scan_interval;
					scan_param.window = params_radio.scan_window;
				    break;
				}
			}
}





int radio_init(void)
{
  	int err;

    err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return 0;
	}

	printk("Bluetooth initialized\n");
    
    return err;
}

int radio_start(void)
{
    int err;
	set_radio_params_init();
	scan_init();
	adv_init();
	set_ble_params(params_radio.mode);
	/* Start advertising */
	err = bt_le_adv_start(BT_LE_ADV_NCONN_IDENTITY, ad, ARRAY_SIZE(ad),
				      NULL, 0);
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return 0;
	}

	
	err = bt_le_scan_start(&scan_param, scan_cb);
	if (err) {
		printk("Starting scanning failed (err %d)\n", err);
		return 0;
	}
    
    return err;
}


int radio_update(void)
{
	int err;

	err = bt_le_adv_update_data(ad, ARRAY_SIZE(ad), NULL, 0);
	return err;
}