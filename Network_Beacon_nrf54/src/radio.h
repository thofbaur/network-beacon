#define ADV_POS_ID 0
#define ADV_POS_NETWORK_STATUS 2

int radio_init(void);
int radio_start(void);
int radio_params_load(void);
int radio_params_save(void);
uint8_t lookup_device_id(const bt_addr_le_t *addr);
void adv_update(uint8_t position, uint8_t value);
