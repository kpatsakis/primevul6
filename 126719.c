void mg_mqtt_sub(struct mg_connection *c, struct mg_str *topic, int qos) {
  static uint16_t s_id;
  uint8_t qos_ = qos & 3;
  uint32_t total_len = 2 + (uint32_t) topic->len + 2 + 1;
  mg_mqtt_send_header(c, MQTT_CMD_SUBSCRIBE, 2, total_len);
  if (++s_id == 0) ++s_id;
  mg_send_u16(c, mg_htons(s_id));
  mg_send_u16(c, mg_htons((uint16_t) topic->len));
  mg_send(c, topic->ptr, topic->len);
  mg_send(c, &qos_, sizeof(qos_));
}