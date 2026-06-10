#define ADV_POS_ID 0
#define ADV_POS_RADIO_STATUS 1
#define ADV_POS_NETWORK_STATUS 2

int radio_init(void);
int radio_start(void);
int radio_params_load(void);
int radio_params_save(void);
int adv_update(void);
