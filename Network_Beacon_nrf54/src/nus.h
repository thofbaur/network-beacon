#ifndef NUS_H
#define NUS_H

#include <stdbool.h>

int nus_service_init(void);
int nus_send_text(const char *text);
bool nus_is_connected(void);

#endif
