static void mg_send_u16(struct mg_connection *c, uint16_t value) {
  mg_send(c, &value, sizeof(value));
}