void network_evaluate_contact(const bt_addr_le_t *addr, 
    int8_t rssi, uint8_t adv_type, struct net_buf_simple *buf);
void network_init(void);
uint8_t network_read_contact(uint8_t *buffer, uint16_t buffer_len);
