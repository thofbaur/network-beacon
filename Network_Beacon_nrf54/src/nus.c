#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <bluetooth/services/nus.h>

#include "nus.h"

static struct bt_conn *current_conn;
static bool nus_notifications_enabled;
static struct bt_gatt_exchange_params mtu_exchange_params;

static void mtu_exchange_cb(struct bt_conn *conn, uint8_t att_err,
			    struct bt_gatt_exchange_params *params)
{
	printk("MTU exchange %s\n", att_err ? "failed" : "successful");
}

static void request_throughput_params(struct bt_conn *conn)
{
	int err;
	const struct bt_conn_le_phy_param phy = {
		.pref_tx_phy = BT_GAP_LE_PHY_2M,
		.pref_rx_phy = BT_GAP_LE_PHY_2M,
	};
	const struct bt_conn_le_data_len_param data_len = {
		.tx_max_len = BT_GAP_DATA_LEN_MAX,
		.tx_max_time = BT_GAP_DATA_TIME_MAX,
	};
	const struct bt_le_conn_param conn_param = {
		.interval_min = BT_GAP_MS_TO_CONN_INTERVAL(7.5),
		.interval_max = BT_GAP_MS_TO_CONN_INTERVAL(15),
		.latency = 0,
		.timeout = BT_GAP_MS_TO_CONN_TIMEOUT(4000),
	};

	mtu_exchange_params.func = mtu_exchange_cb;
	err = bt_gatt_exchange_mtu(conn, &mtu_exchange_params);
	if (err) {
		printk("MTU exchange request failed (err %d)\n", err);
	}

	err = bt_conn_le_phy_update(conn, &phy);
	if (err) {
		printk("PHY update request failed (err %d)\n", err);
	}

	err = bt_conn_le_data_len_update(conn, &data_len);
	if (err) {
		printk("Data length update request failed (err %d)\n", err);
	}

	err = bt_conn_le_param_update(conn, &conn_param);
	if (err) {
		printk("Connection parameter update request failed (err %d)\n", err);
	}
}

static void send_uptime(struct bt_conn *conn)
{
	char response[32];
	uint32_t uptime_s;
	int len;

	uptime_s = (uint32_t)k_uptime_seconds();
	len = snprintf(response, sizeof(response), "%u\r\n", uptime_s);
	if (len < 0 || len >= sizeof(response)) {
		return;
	}

	if (bt_nus_send(conn, response, len)) {
		printk("Failed to send NUS uptime response\n");
	}
}

static void nus_received(struct bt_conn *conn, const uint8_t *const data, uint16_t len)
{
	ARG_UNUSED(data);
	ARG_UNUSED(len);

	send_uptime(conn);
}

static void nus_send_enabled(enum bt_nus_send_status status)
{
	nus_notifications_enabled = (status == BT_NUS_SEND_STATUS_ENABLED);
}

static struct bt_nus_cb nus_cb = {
	.received = nus_received,
	.send_enabled = nus_send_enabled,
};

static void connected(struct bt_conn *conn, uint8_t err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	if (err) {
		printk("Connection failed (err 0x%02x)\n", err);
		return;
	}

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	printk("Connected: %s\n", addr);

	if (current_conn) {
		bt_conn_unref(current_conn);
	}

	current_conn = bt_conn_ref(conn);
	request_throughput_params(conn);
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	printk("Disconnected: %s (reason 0x%02x)\n", addr, reason);

	if (current_conn == conn) {
		bt_conn_unref(current_conn);
		current_conn = NULL;
	}

	nus_notifications_enabled = false;
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};

int nus_service_init(void)
{
	int err;

	err = bt_nus_init(&nus_cb);
	if (err) {
		printk("Failed to initialize NUS (err %d)\n", err);
	}

	return err;
}

int nus_send_text(const char *text)
{
	if (!current_conn || !nus_notifications_enabled) {
		return -ENOTCONN;
	}

	return bt_nus_send(current_conn, text, strlen(text));
}

bool nus_is_connected(void)
{
	return current_conn != NULL;
}
