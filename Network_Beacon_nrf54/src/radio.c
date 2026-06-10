#include <errno.h>
#include <string.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/util.h>
//#include <bluetooth/scan.h>
#include "defines.h"
#include "radio_ids.h"
#include "network.h"
#include "param_storage.h"
#include "radio.h"
#include "nus.h"
#include "device.h"
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
#define COMMAND_TARGET_BROADCAST	0xff
#define COMMAND_DATA_MAX_LEN		31
#define COMMAND_QUEUE_DEPTH		4
#define RADIO_PARAMS_STORAGE_KEY	"dsa/radio"
#define RADIO_STATUS_SCAN_ERROR	BIT(0)



static uint8_t mfg_data[] = { 0xff, 0x00, 0x00 };

static const struct bt_data ad[] = {
	//BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, strlen(CONFIG_BT_DEVICE_NAME)),
	BT_DATA(BT_DATA_MANUFACTURER_DATA, mfg_data, sizeof(mfg_data)),
};

struct radio_params {
	uint16_t	adv_interval_min;
	uint16_t	adv_interval_max;
	uint16_t	adv_interval_min_lowactivity;
	uint16_t	adv_interval_max_lowactivity;
	uint16_t	scan_interval;
	uint16_t	scan_interval_lowactivity;
	uint16_t	scan_window;
	uint16_t	scan_window_lowactivity;
	uint8_t		mode;
};

static struct radio_params params_radio;

static struct bt_le_scan_param scan_params;
static struct bt_le_adv_param adv_params;

enum target_action {
    ACTION_NONE = 0,
    ACTION_DSA,
    ACTION_DST,
    ACTION_DSZ,
};

struct target_device {
    const char *name;
    enum target_action action;
};

static const struct target_device target_devices[] = {
    { .name = "DSA",    .action = ACTION_DSA },
    { .name = "DSZ", 	.action = ACTION_DSZ },
    { .name = "DST", 	.action = ACTION_DST },
};
struct name_check {
    	bool found;
    	enum target_action action;
	};

struct command_msg {
	uint8_t len;
	uint8_t data[COMMAND_DATA_MAX_LEN];
};

static void command_work_handler(struct k_work *work);
static void radio_status_set(uint8_t mask, bool active);

K_MSGQ_DEFINE(command_msgq, sizeof(struct command_msg), COMMAND_QUEUE_DEPTH, 1);
static K_WORK_DEFINE(command_work, command_work_handler);

bool radio_params_hardcoded=0;

void set_ble_params(struct radio_params *params);
static int update_ble_params(struct bt_le_scan_param *scan_params, struct bt_le_adv_param *adv_params);
static int radio_params_validate(const struct radio_params *params);



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

static bool name_check_cb(struct bt_data *data, void *user_data)
{
    struct name_check *check = user_data;

    if (data->type != BT_DATA_NAME_COMPLETE &&
        data->type != BT_DATA_NAME_SHORTENED) {
        return true;
    }

    for (size_t i = 0; i < ARRAY_SIZE(target_devices); i++) {
        const char *target_name = target_devices[i].name;

        if (data->data_len == strlen(target_name) &&
            memcmp(data->data, target_name, data->data_len) == 0) {
            check->found = true;
            check->action = target_devices[i].action;
            return false;
        }
    }

    return true;
}

static bool scan_extract_data(struct bt_data *data, void *user_data)
{
	struct net_buf_simple *buf = user_data;

	if (data->type == BT_DATA_MANUFACTURER_DATA)
	{
		uint16_t copy_len = MIN(data->data_len, net_buf_simple_tailroom(buf));

		net_buf_simple_add_mem(buf, data->data, copy_len);
		if (copy_len < data->data_len) {
			printk("Command data truncated from %u to %u bytes\n",
			       data->data_len, copy_len);
		}
		return false; // Stop parsing further
	}

	return true; // Continue parsing other data fields    
 
}

static void radio_apply_command(uint8_t parameter, uint16_t value)
{
	int err;
	struct radio_params old_params_radio = params_radio;

	switch (parameter) {
	case P_ADV_INTERVAL_MS:
		params_radio.adv_interval_min = BT_GAP_MS_TO_ADV_INTERVAL(value);
		params_radio.adv_interval_max = BT_GAP_MS_TO_ADV_INTERVAL(value);
		break;
	case P_ADV_INTERVAL_LOWACTIVITY_MS:
		params_radio.adv_interval_min_lowactivity = BT_GAP_MS_TO_ADV_INTERVAL(value);
		params_radio.adv_interval_max_lowactivity = BT_GAP_MS_TO_ADV_INTERVAL(value);
		break;
	case P_SCAN_INTERVAL_MS:
		params_radio.scan_interval = BT_GAP_MS_TO_SCAN_INTERVAL(value);
		break;
	case P_SCAN_INTERVAL_LOWACTIVITY_MS:
		params_radio.scan_interval_lowactivity = BT_GAP_MS_TO_SCAN_INTERVAL(value);
		break;
	case P_SCAN_WINDOW_MS:
		params_radio.scan_window = BT_GAP_MS_TO_SCAN_WINDOW(value);
		break;
	case P_SCAN_WINDOW_LOWACTIVITY_MS:
		params_radio.scan_window_lowactivity = BT_GAP_MS_TO_SCAN_WINDOW(value);
		break;
	case P_RADIO_RESET_PARAMS:
		set_radio_params_init();
		break;
	case P_SET_RAD_ACTIVE:
		params_radio.mode = value ? HIGH_ACTIVITY : LOW_ACTIVITY;
		break;
	default:
		printk("Unknown radio parameter 0x%02x value %u\n", parameter, value);
		break;
	}

	if (memcmp(&old_params_radio, &params_radio, sizeof(params_radio)) != 0) {
		err = radio_params_validate(&params_radio);
		if (err) {
			printk("Rejecting invalid radio parameters (err %d)\n", err);
			params_radio = old_params_radio;
			return;
		}

		set_ble_params(&params_radio);
		err = update_ble_params(&scan_params, &adv_params);
		if (err) {
			printk("Failed to apply radio parameters (err %d), restoring old values\n", err);
			params_radio = old_params_radio;
			set_ble_params(&params_radio);
			update_ble_params(&scan_params, &adv_params);
			return;
		}

		err = radio_params_save();
		if (err) {
			printk("Failed to save radio parameters (err %d)\n", err);
		}
	}
}

int radio_params_load(void)
{
	return param_storage_load(RADIO_PARAMS_STORAGE_KEY,
				  &params_radio, sizeof(params_radio));
}

int radio_params_save(void)
{
	return param_storage_save(RADIO_PARAMS_STORAGE_KEY,
				  &params_radio, sizeof(params_radio));
}

static void main_apply_command(uint8_t parameter, uint16_t value)
{
	switch (parameter) {
	case P_MAIN_LED_ACTIVE:
		printk("Main LED command value %u not implemented\n", value);
		break;
	case P_MAIN_RESET_PARAMS:
		printk("Main parameter reset command not implemented\n");
		break;
	default:
		printk("Unknown main parameter 0x%02x value %u\n", parameter, value);
		break;
	}
}

static void evaluate_command_data(const uint8_t *data, uint8_t len)
{
	for (uint8_t offset = 0; offset + 2 < len; offset += 3) {
		uint8_t parameter = data[offset];
		uint16_t value = sys_get_be16(&data[offset + 1]);

		printk("Command parameter 0x%02x value %u\n", parameter, value);

		switch (parameter & P_BASE_MASK) {
		case P_BASE_MAIN:
			main_apply_command(parameter, value);
			break;
		case P_BASE_NETWORK:
			network_apply_command(parameter, value);
			break;
		case P_BASE_RADIO:
			radio_apply_command(parameter, value);
			break;
		default:
			printk("Unknown parameter base 0x%02x for parameter 0x%02x\n",
			       parameter & P_BASE_MASK, parameter);
			break;
		}
	}

	if ((len % 3) != 0) {
		printk("Ignoring %u incomplete command byte(s)\n", len % 3);
	}
}

static void radio_evaluate_command_data(const uint8_t *data, uint8_t len)
{
	uint8_t target;

	if (len < 1) {
		printk("Command data missing target byte\n");
		return;
	}

	target = data[0];
	if (target != mfg_data[ADV_POS_ID] && target != COMMAND_TARGET_BROADCAST) {
		printk("Ignoring command for target 0x%02x, own id 0x%02x\n",
		       target, mfg_data[ADV_POS_ID]);
		return;
	}

	evaluate_command_data(&data[1], len - 1);
}

static void command_work_handler(struct k_work *work)
{
	struct command_msg msg;

	ARG_UNUSED(work);

	while (k_msgq_get(&command_msgq, &msg, K_NO_WAIT) == 0) {
		radio_evaluate_command_data(msg.data, msg.len);
	}
}

static void enqueue_command_from_ad(struct net_buf_simple *buf)
{
	int err;
	struct command_msg msg = { 0 };
	struct net_buf_simple ad_temp;
	NET_BUF_SIMPLE_DEFINE(command_buf, COMMAND_DATA_MAX_LEN);

	net_buf_simple_clone(buf, &ad_temp);
	bt_data_parse(&ad_temp, scan_extract_data, &command_buf);

	if (command_buf.len == 0) {
		printk("Command advertisement has no manufacturer data\n");
		return;
	}

	msg.len = command_buf.len;
	memcpy(msg.data, command_buf.data, msg.len);

	err = k_msgq_put(&command_msgq, &msg, K_NO_WAIT);
	if (err) {
		printk("Command queue full, dropping command (err %d)\n", err);
		return;
	}

	k_work_submit(&command_work);
}

static void scan_cb(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type,
		    struct net_buf_simple *buf)
{
	struct net_buf_simple ad_temp;
	struct name_check check = {
		.action = ACTION_NONE,
		.found = false,
	};
	net_buf_simple_clone(buf, &ad_temp);
	bt_data_parse(&ad_temp, name_check_cb, &check);

	if (check.found) {
		printk("Found target device DSA\n");
		switch (check.action) {
		case ACTION_DSA:
		case ACTION_DST:
			network_evaluate_contact(addr, rssi, adv_type, buf);	
			break;

		case ACTION_DSZ:
			enqueue_command_from_ad(buf);
			break;
		default:
			break;
		}	
	}
    /* Do follow-up logic here, not inside name_check_cb */
}

static int radio_params_validate(const struct radio_params *params)
{
	// TODO: Add validation for parameter ranges if needed
	if (params->mode != HIGH_ACTIVITY && params->mode != LOW_ACTIVITY) {
		return -EINVAL;
	}

	if (params->adv_interval_min == 0 ||
	    params->adv_interval_max == 0 ||
	    params->adv_interval_min_lowactivity == 0 ||
	    params->adv_interval_max_lowactivity == 0 ||
	    params->scan_interval == 0 ||
	    params->scan_interval_lowactivity == 0 ||
	    params->scan_window == 0 ||
	    params->scan_window_lowactivity == 0) {
		return -EINVAL;
	}

	if (params->adv_interval_min > params->adv_interval_max ||
	    params->adv_interval_min_lowactivity > params->adv_interval_max_lowactivity) {
		return -EINVAL;
	}

	if (params->scan_window > params->scan_interval ||
	    params->scan_window_lowactivity > params->scan_interval_lowactivity) {
		return -EINVAL;
	}

	return 0;
}

static int update_ble_params(struct bt_le_scan_param *parameters_scan, struct bt_le_adv_param *parameters_adv)
{
	int err;
	int first_err = 0;

	err = bt_le_adv_stop();
	if (err && err != -EALREADY) {
		printk("Advertising stop before parameter update failed (err %d)\n", err);
		first_err = err;
	}

	err = bt_le_scan_stop();
	if (err && err != -EALREADY) {
		printk("Scan stop before parameter update failed (err %d)\n", err);
		if (!first_err) {
			first_err = err;
		}
	}

	err = bt_le_adv_start(parameters_adv, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err && err != -EALREADY) {
		printk("Advertising parameter update failed (err %d)\n", err);
		if (!first_err) {
			first_err = err;
		}
	}

	err = bt_le_scan_start(parameters_scan, scan_cb);
	if (err && err != -EALREADY) {
		printk("Scan parameter update failed (err %d)\n", err);
		radio_status_set(RADIO_STATUS_SCAN_ERROR, true);
		if (!first_err) {
			first_err = err;
		}
	} else {
		radio_status_set(RADIO_STATUS_SCAN_ERROR, false);
	}

	return first_err;
}

void scan_init(void)
{
    int err;


	for (size_t i = 0; i < ARRAY_SIZE(known_device_table); i++) 
	{
    	err = bt_le_filter_accept_list_add(&known_device_table[i].addr);
		if (err) {
    	    printk("Failed to add to filter list (err %d)\n", err);
		return;
    }
		}
}

void adv_init(void)
{
	adv_params.id = 0U;
	adv_params.sid = 0U;
	adv_params.secondary_max_skip = 0U;
	adv_params.options = BT_LE_ADV_OPT_CONN | BT_LE_ADV_OPT_USE_IDENTITY;
	mfg_data[ADV_POS_ID] = get_device_id();
}

void adv_update(uint8_t position, uint8_t value)
{
	int err;

	if (position < sizeof(mfg_data)) {
		mfg_data[position] = value;
	}

	err = bt_le_adv_update_data(ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("Advertising data update failed (err %d)\n", err);
	}
}

static void radio_status_set(uint8_t mask, bool active)
{
	uint8_t status = mfg_data[ADV_POS_RADIO_STATUS];

	if (active) {
		status |= mask;
	} else {
		status &= ~mask;
	}

	adv_update(ADV_POS_RADIO_STATUS, status);
}

void set_ble_params(struct radio_params *params)
{
	scan_params.type = BT_LE_SCAN_TYPE_PASSIVE;
	scan_params.options = BT_LE_SCAN_OPT_FILTER_ACCEPT_LIST;
	switch(params->mode)
			{
				case LOW_ACTIVITY:
				{
				    adv_params.interval_min   = params->adv_interval_min_lowactivity;
					adv_params.interval_max   = params->adv_interval_max_lowactivity;
				    scan_params.interval = params->scan_interval_lowactivity;
				    scan_params.window = params->scan_window_lowactivity;
					break;
				}
				case HIGH_ACTIVITY:
				{
					adv_params.interval_min   = params->adv_interval_min;
					adv_params.interval_max   = params->adv_interval_max;
					scan_params.interval = params->scan_interval;
					scan_params.window = params->scan_window;
				    break;
				}
			}
}

int radio_init(void)
{
  	int err;
	int load_err;

	err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return err;
	}

	printk("Bluetooth initialized\n");

	err = nus_service_init();
	if (err) {
		return err;
	}

	printk("NUS initialized\n");
	set_radio_params_init();
	load_err = radio_params_load();
	if (load_err == -ENOENT) {
		printk("No stored radio parameters, using defaults\n");
	} else if (load_err) {
		printk("Failed to load radio parameters (err %d), using defaults\n", load_err);
	}
	scan_init();
	adv_init();
	return err;
}

int radio_start(void)
{
    int err;

	set_ble_params(&params_radio);
	/* Start advertising */
	err = bt_le_adv_start(&adv_params, ad, ARRAY_SIZE(ad),
				      NULL, 0);
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return err;
	}

	
	err = bt_le_scan_start(&scan_params, scan_cb);
	if (err) {
		printk("Starting scanning failed (err %d)\n", err);
		radio_status_set(RADIO_STATUS_SCAN_ERROR, true);
		return err;
	}

	radio_status_set(RADIO_STATUS_SCAN_ERROR, false);
    
    return 0;
}


static void radio_disconnected(struct bt_conn *conn, uint8_t reason)
{
	ARG_UNUSED(conn);

	printk("Connection disconnected, advertising restart waits for recycled callback (reason 0x%02x)\n",
	       reason);
}

static void radio_recycled(void)
{
	int err;

	printk("Connection object recycled, restarting advertising\n");
	err = bt_le_adv_start(&adv_params, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("Advertising failed to restart (err %d)\n", err);
	} else {
		printk("Advertising restarted\n");
	}
}

BT_CONN_CB_DEFINE(radio_conn_callbacks) = {
	.disconnected = radio_disconnected,
	.recycled = radio_recycled,
};
