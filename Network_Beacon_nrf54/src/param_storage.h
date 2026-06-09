#pragma once

#include <stddef.h>

int param_storage_load(const char *key, void *data, size_t len);
int param_storage_save(const char *key, const void *data, size_t len);
