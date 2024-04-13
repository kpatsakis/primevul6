void mg_mqtt_login(struct mg_connection *c, struct mg_mqtt_opts *opts) {
  char rnd[9], client_id[16];
  struct mg_str cid = opts->client_id;
  uint32_t total_len = 7 + 1 + 2 + 2;
  uint8_t connflag = (uint8_t) ((opts->will_qos & 3) << 3);

  if (cid.len == 0) {
    mg_random(rnd, sizeof(rnd));
    mg_base64_encode((unsigned char *) rnd, sizeof(rnd), client_id);
    client_id[sizeof(client_id) - 1] = '\0';
    cid = mg_str(client_id);
  }

  if (opts->user.len > 0) {
    total_len += 2 + (uint32_t) opts->user.len;
    connflag |= MQTT_HAS_USER_NAME;
  }
  if (opts->pass.len > 0) {
    total_len += 2 + (uint32_t) opts->pass.len;
    connflag |= MQTT_HAS_PASSWORD;
  }
  if (opts->will_topic.len > 0 && opts->will_message.len > 0) {
    total_len +=
        4 + (uint32_t) opts->will_topic.len + (uint32_t) opts->will_message.len;
    connflag |= MQTT_HAS_WILL;
  }
  if (opts->clean || cid.len == 0) connflag |= MQTT_CLEAN_SESSION;
  if (opts->will_retain) connflag |= MQTT_WILL_RETAIN;
  total_len += (uint32_t) cid.len;

  mg_mqtt_send_header(c, MQTT_CMD_CONNECT, 0, total_len);
  mg_send(c, "\00\04MQTT\04", 7);
  mg_send(c, &connflag, sizeof(connflag));
  // keepalive == 0 means "do not disconnect us!"
  mg_send_u16(c, mg_htons((uint16_t) opts->keepalive));
  mg_send_u16(c, mg_htons((uint16_t) cid.len));
  mg_send(c, cid.ptr, cid.len);
  if (connflag & MQTT_HAS_WILL) {
    mg_send_u16(c, mg_htons((uint16_t) opts->will_topic.len));
    mg_send(c, opts->will_topic.ptr, opts->will_topic.len);
    mg_send_u16(c, mg_htons((uint16_t) opts->will_message.len));
    mg_send(c, opts->will_message.ptr, opts->will_message.len);
  }
  if (opts->user.len > 0) {
    mg_send_u16(c, mg_htons((uint16_t) opts->user.len));
    mg_send(c, opts->user.ptr, opts->user.len);
  }
  if (opts->pass.len > 0) {
    mg_send_u16(c, mg_htons((uint16_t) opts->pass.len));
    mg_send(c, opts->pass.ptr, opts->pass.len);
  }
}