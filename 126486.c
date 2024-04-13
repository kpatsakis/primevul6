void mg_mqtt_pub(struct mg_connection *c, struct mg_str *topic,
                 struct mg_str *data, int qos, bool retain) {
  uint8_t flags = (uint8_t) (((qos & 3) << 1) | (retain ? 1 : 0));
  uint32_t total_len = 2 + (uint32_t) topic->len + (uint32_t) data->len;
  LOG(LL_DEBUG, ("%lu [%.*s] -> [%.*s]", c->id, (int) topic->len,
                 (char *) topic->ptr, (int) data->len, (char *) data->ptr));
  if (qos > 0) total_len += 2;
  mg_mqtt_send_header(c, MQTT_CMD_PUBLISH, flags, total_len);
  mg_send_u16(c, mg_htons((uint16_t) topic->len));
  mg_send(c, topic->ptr, topic->len);
  if (qos > 0) {
    static uint16_t s_id;
    if (++s_id == 0) s_id++;
    mg_send_u16(c, mg_htons(s_id));
  }
  mg_send(c, data->ptr, data->len);
}