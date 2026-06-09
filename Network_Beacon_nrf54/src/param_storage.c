#include <errno.h>
#include <stdbool.h>
#include <stddef.h>

#include <zephyr/settings/settings.h>
#include <zephyr/sys/printk.h>

#include "param_storage.h"

static bool settings_initialized;

static int param_storage_init(void)
{
	int err;

	if (settings_initialized) {
		return 0;
	}

	err = settings_subsys_init();
	if (err) {
		printk("Settings init failed (err %d)\n", err);
		return err;
	}

	settings_initialized = true;
	return 0;
}

int param_storage_load(const char *key, void *data, size_t len)
{
	ssize_t loaded;
	int err;

	err = param_storage_init();
	if (err) {
		return err;
	}

	loaded = settings_load_one(key, data, len);
	if (loaded < 0) {
		return loaded;
	}

	if ((size_t)loaded != len) {
		return -EINVAL;
	}

	return 0;
}

int param_storage_save(const char *key, const void *data, size_t len)
{
	int err;

	err = param_storage_init();
	if (err) {
		return err;
	}

	return settings_save_one(key, data, len);
}
