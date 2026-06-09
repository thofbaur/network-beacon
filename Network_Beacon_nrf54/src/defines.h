#define DSA_NUS_FLAG_TIME     0x01
#define DSA_NUS_FLAG_DATA       0x02
#define DSA_NUS_FLAG_VOLTAGE    0x03

#define P_NULL				0
// Param_Base_Mask = 0xE0 -> Possible Values: 000 ... 111 << 5
#define P_BASE_MASK			0xE0
#define P_BASE_MAIN			0x20
#define P_BASE_NETWORK		0x60
#define P_BASE_RADIO		0x80

// Defines for Main Parameters should be in one block
#define P_MAIN_LED_ACTIVE       P_BASE_MAIN+1
#define P_MAIN_RESET_PARAMS		P_BASE_MAIN+12

// Defines for Network Parameters should be in one block
#define P_RSSI_NETWORK		    P_BASE_NETWORK+4
#define P_NETWORK_RESET_PARAMS 	P_BASE_NETWORK+12
#define P_TRACKING_ACTIVE 	    P_BASE_NETWORK+13

// Defines for Radio Parameters should be in one block
#define P_ADV_INTERVAL_MS			P_BASE_RADIO+1
#define P_ADV_INTERVAL_LOWACTIVITY_MS	P_BASE_RADIO+2
#define P_SCAN_INTERVAL_MS			P_BASE_RADIO+3
#define P_SCAN_INTERVAL_LOWACTIVITY_MS	P_BASE_RADIO+4
#define P_SCAN_WINDOW_MS			P_BASE_RADIO+5
#define P_SCAN_WINDOW_LOWACTIVITY_MS	P_BASE_RADIO+6
#define P_RADIO_RESET_PARAMS	P_BASE_RADIO+12
#define P_SET_RAD_ACTIVE		P_BASE_RADIO+13
