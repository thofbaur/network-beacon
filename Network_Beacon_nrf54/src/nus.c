#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/util.h>

#include <bluetooth/services/nus.h>

#include "nus.h"
#include "defines.h"
#include "network.h"

#define NUS_CONNECTION_TIMEOUT_MS 20000
#define NUS_ATT_NOTIFY_HEADER_LEN 3
#define NUS_MAX_PAYLOAD_LEN 244
#define NUS_PENDING_SEND_DEPTH 16

static struct bt_conn *current_conn;
static bool nus_notifications_enabled;
static bool disconnect_when_sent;
static atomic_t pending_nus_sends;
static struct bt_gatt_exchange_params mtu_exchange_params;

struct pending_nus_send {
	bool is_contact_data;
	uint8_t contact_len;
	uint8_t contact_data[NUS_MAX_PAYLOAD_LEN - 1];
};

static struct pending_nus_send pending_sends[NUS_PENDING_SEND_DEPTH];
static uint8_t pending_send_head;
static uint8_t pending_send_count;
static K_MUTEX_DEFINE(pending_send_lock);

static void connection_timeout_handler(struct k_work *work);

static K_WORK_DELAYABLE_DEFINE(connection_timeout_work, connection_timeout_handler);

static bool pending_send_full(void)
{
	bool full;

	k_mutex_lock(&pending_send_lock, K_FOREVER);
	full = pending_send_count == ARRAY_SIZE(pending_sends);
	k_mutex_unlock(&pending_send_lock);

	return full;
}

static void pending_send_push(bool is_contact_data, const uint8_t *contact_data,
			      uint8_t contact_len)
{
	uint8_t index;

	k_mutex_lock(&pending_send_lock, K_FOREVER);

	index = (pending_send_head + pending_send_count) % ARRAY_SIZE(pending_sends);
	pending_sends[index].is_contact_data = is_contact_data;
	pending_sends[index].contact_len = contact_len;
	if (is_contact_data && contact_len > 0) {
		memcpy(pending_sends[index].contact_data, contact_data, contact_len);
	}
	pending_send_count++;

	k_mutex_unlock(&pending_send_lock);
}

static bool pending_send_pop(struct pending_nus_send *send)
{
	bool found = false;

	k_mutex_lock(&pending_send_lock, K_FOREVER);

	if (pending_send_count > 0) {
		*send = pending_sends[pending_send_head];
		pending_send_head = (pending_send_head + 1) % ARRAY_SIZE(pending_sends);
		pending_send_count--;
		found = true;
	}

	k_mutex_unlock(&pending_send_lock);

	return found;
}

static void pending_send_restore_contacts(void)
{
	uint8_t count;
	uint8_t index;
	struct pending_nus_send send;

	k_mutex_lock(&pending_send_lock, K_FOREVER);
	count = pending_send_count;
	k_mutex_unlock(&pending_send_lock);

	while (count > 0) {
		k_mutex_lock(&pending_send_lock, K_FOREVER);
		index = (pending_send_head + count - 1) % ARRAY_SIZE(pending_sends);
		send = pending_sends[index];
		k_mutex_unlock(&pending_send_lock);

		if (send.is_contact_data && send.contact_len > 0) {
			network_restore_contact(send.contact_data, send.contact_len);
		}

		count--;
	}

	k_mutex_lock(&pending_send_lock, K_FOREVER);
	pending_send_head = 0;
	pending_send_count = 0;
	k_mutex_unlock(&pending_send_lock);
}

static int nus_send_tracked(struct bt_conn *conn, const void *data, uint16_t len,
			    const uint8_t *contact_data, uint8_t contact_len)
{
	int err;
	bool is_contact_data = contact_data != NULL && contact_len > 0;

	if (pending_send_full()) {
		return -ENOMEM;
	}

	err = bt_nus_send(conn, data, len);
	if (err) {
		return err;
	}

	pending_send_push(is_contact_data, contact_data, contact_len);
	atomic_inc(&pending_nus_sends);

	return 0;
}

static void disconnect_nus_connection(struct bt_conn *conn)
{
	if (bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN)) {
		printk("Failed to disconnect NUS connection\n");
	}
}

static void refresh_connection_timeout(void)
{
	if (NUS_CONNECTION_TIMEOUT_MS == 0) {
		return;
	}

	k_work_reschedule(&connection_timeout_work, K_MSEC(NUS_CONNECTION_TIMEOUT_MS));
}

static void cancel_connection_timeout(void)
{
	k_work_cancel_delayable(&connection_timeout_work);
}

static void connection_timeout_handler(struct k_work *work)
{
	struct bt_conn *conn;
	int err;

	ARG_UNUSED(work);

	if (!current_conn) {
		return;
	}

	conn = bt_conn_ref(current_conn);
	printk("No NUS data received for %u ms, disconnecting\n", NUS_CONNECTION_TIMEOUT_MS);

	err = bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
	if (err) {
		printk("Connection timeout disconnect failed (err %d)\n", err);
	}

	bt_conn_unref(conn);
}

static void print_conn_info(struct bt_conn *conn, const char *prefix)
{
	struct bt_conn_info info;
	int err;

	err = bt_conn_get_info(conn, &info);
	if (err) {
		printk("%s: failed to read connection info (err %d)\n", prefix, err);
		return;
	}

	if (info.type != BT_CONN_TYPE_LE) {
		printk("%s: non-LE connection type %u\n", prefix, info.type);
		return;
	}

	printk("%s: role=%s interval=%u us latency=%u timeout=%u units\n",
	       prefix,
	       info.role == BT_CONN_ROLE_PERIPHERAL ? "peripheral" : "central",
	       info.le.interval_us, info.le.latency, info.le.timeout);
}

static void mtu_exchange_cb(struct bt_conn *conn, uint8_t att_err,
			    struct bt_gatt_exchange_params *params)
{
	printk("MTU exchange %s", att_err ? "failed" : "successful");
	if (att_err) {
		printk(" (ATT err 0x%02x)", att_err);
	}
	printk(", negotiated MTU %u\n", bt_gatt_get_mtu(conn));
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

	printk("Requesting high-throughput connection parameters\n");
	printk("Requested PHY: TX=2M RX=2M\n");
	printk("Requested data length: tx_max_len=%u tx_max_time=%u\n",
	       data_len.tx_max_len, data_len.tx_max_time);
	printk("Requested connection interval: %u-%u units, latency=%u, timeout=%u units\n",
	       conn_param.interval_min, conn_param.interval_max,
	       conn_param.latency, conn_param.timeout);

	mtu_exchange_params.func = mtu_exchange_cb;
	err = bt_gatt_exchange_mtu(conn, &mtu_exchange_params);
	if (err) {
		printk("MTU exchange request failed (err %d)\n", err);
	} else {
		printk("MTU exchange request sent\n");
	}

	err = bt_conn_le_phy_update(conn, &phy);
	if (err) {
		printk("PHY update request failed (err %d)\n", err);
	} else {
		printk("PHY update request sent\n");
	}

	err = bt_conn_le_data_len_update(conn, &data_len);
	if (err) {
		printk("Data length update request failed (err %d)\n", err);
	} else {
		printk("Data length update request sent\n");
	}

	err = bt_conn_le_param_update(conn, &conn_param);
	if (err) {
		printk("Connection parameter update request failed (err %d)\n", err);
	} else {
		printk("Connection parameter update request sent\n");
	}
}

static void send_uptime(struct bt_conn *conn)
{
	uint8_t buffer[1 + sizeof(uint32_t)];

	buffer[0] = DSA_NUS_FLAG_TIME;
	sys_put_be32((uint32_t)k_uptime_seconds(), &buffer[1]);

	if (nus_send_tracked(conn, buffer, sizeof(buffer), NULL, 0)) {
		printk("Failed to send NUS uptime response\n");
	}
}

static void send_networkdata(struct bt_conn *conn)
{
	uint16_t max_payload = bt_gatt_get_mtu(conn);
	uint16_t payload_len;
	uint8_t bytes_written = 0;
	uint16_t contact_payload_len;
	uint8_t buffer[NUS_MAX_PAYLOAD_LEN];
	
	if (max_payload > NUS_ATT_NOTIFY_HEADER_LEN) {
		max_payload -= NUS_ATT_NOTIFY_HEADER_LEN;
	} else {
		max_payload = 0;
	}

	printk("NUS max payload: %u bytes\n", max_payload);

	payload_len = MIN(max_payload, (uint16_t)sizeof(buffer));
	if (payload_len <= 1) {
		return;
	}

	contact_payload_len = payload_len - 1;
	buffer[0] = DSA_NUS_FLAG_DATA;

	do {
		bytes_written = network_read_contact(&buffer[1], contact_payload_len);
		if (bytes_written > 0) {
			if (nus_send_tracked(conn, buffer, bytes_written + 1,
					     &buffer[1], bytes_written)) {
				printk("Failed to send NUS network data\n");
				network_restore_contact(&buffer[1], bytes_written);
				return;
			}
		}
	} while (bytes_written > 0);
}


static void nus_received(struct bt_conn *conn, const uint8_t *const data, uint16_t len)
{
	refresh_connection_timeout();

	printk("NUS RX len=%u first=%02x %02x %02x %02x\n", len,
	       len > 0 ? data[0] : 0,
	       len > 1 ? data[1] : 0,
	       len > 2 ? data[2] : 0,
	       len > 3 ? data[3] : 0);

	if (len == 2 && memcmp(data, "st", 2) == 0) {
		printk("Starting sending data\n");
		send_uptime(conn);
		printk("Sent time\n");
		send_networkdata(conn);

		if (nus_send_tracked(conn, "finished", strlen("finished"), NULL, 0)) {
			printk("Failed to send NUS finished response\n");
		} else {
			disconnect_when_sent = true;
		}

	}
}

static void nus_sent(struct bt_conn *conn)
{
	struct pending_nus_send send;

	if (!pending_send_pop(&send)) {
		printk("NUS sent callback without pending send\n");
		return;
	}

	if (atomic_get(&pending_nus_sends) > 0) {
		atomic_dec(&pending_nus_sends);
	}

	if (disconnect_when_sent && atomic_get(&pending_nus_sends) == 0) {
		disconnect_when_sent = false;
		disconnect_nus_connection(conn);
	}
}

static void nus_send_enabled(enum bt_nus_send_status status)
{
	nus_notifications_enabled = (status == BT_NUS_SEND_STATUS_ENABLED);
	printk("NUS TX notifications %s\n", nus_notifications_enabled ? "enabled" : "disabled");
}

static struct bt_nus_cb nus_cb = {
	.received = nus_received,
	.sent = nus_sent,
	.send_enabled = nus_send_enabled,
};

static void connected(struct bt_conn *conn, uint8_t err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	printk("Connection event from %s\n", addr);

	if (err) {
		printk("Connection failed from %s (HCI err 0x%02x)\n", addr, err);
		return;
	}

	printk("Connected: %s\n", addr);
	print_conn_info(conn, "Initial connection parameters");

	if (current_conn) {
		printk("Replacing previous connection reference\n");
		bt_conn_unref(current_conn);
	}

	current_conn = bt_conn_ref(conn);
	refresh_connection_timeout();
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

	cancel_connection_timeout();
	pending_send_restore_contacts();
	nus_notifications_enabled = false;
	disconnect_when_sent = false;
	atomic_set(&pending_nus_sends, 0);
}

static void le_param_updated(struct bt_conn *conn, uint16_t interval,
			     uint16_t latency, uint16_t timeout)
{
	ARG_UNUSED(conn);

	printk("Connection parameters updated: interval=%u units latency=%u timeout=%u units\n",
	       interval, latency, timeout);
}

static void le_phy_updated(struct bt_conn *conn, struct bt_conn_le_phy_info *param)
{
	ARG_UNUSED(conn);

	printk("PHY updated: TX=%u RX=%u\n", param->tx_phy, param->rx_phy);
}

static void le_data_len_updated(struct bt_conn *conn,
				struct bt_conn_le_data_len_info *info)
{
	ARG_UNUSED(conn);

	printk("Data length updated: tx_len=%u tx_time=%u rx_len=%u rx_time=%u\n",
	       info->tx_max_len, info->tx_max_time,
	       info->rx_max_len, info->rx_max_time);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
	.le_param_updated = le_param_updated,
	.le_phy_updated = le_phy_updated,
	.le_data_len_updated = le_data_len_updated,
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
