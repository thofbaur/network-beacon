#ifndef LED_H
#define LED_H

#include <stdint.h>

void led_init(void);
void led_apply_command(uint8_t parameter, uint16_t value);
int led_params_load(void);
int led_params_save(void);

#endif /* LED_H */
