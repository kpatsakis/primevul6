static void mqtt_cb(struct mg_connection *c, int ev, void *ev_data,
                    void *fn_data) {
  if (ev == MG_EV_READ) {
    for (;;) {
      struct mg_mqtt_message mm;
      int rc = mg_mqtt_parse(c->recv.buf, c->recv.len, &mm);
      if (rc == MQTT_MALFORMED) {
        LOG(LL_ERROR, ("%lu MQTT malformed message", c->id));
        c->is_closing = 1;
        break;
      } else if (rc == MQTT_OK) {
        LOG(LL_VERBOSE_DEBUG,
            ("%p MQTT CMD %d len %d [%.*s]", c->fd, mm.cmd, (int) mm.dgram.len,
             (int) mm.data.len, mm.data.ptr));
        switch (mm.cmd) {
          case MQTT_CMD_CONNACK:
            mg_call(c, MG_EV_MQTT_OPEN, &mm.ack);
            if (mm.ack == 0) {
              LOG(LL_INFO, ("%lu Connected", c->id));
            } else {
              LOG(LL_ERROR, ("%lu MQTT auth failed, code %d", c->id, mm.ack));
              c->is_closing = 1;
            }
            break;
          case MQTT_CMD_PUBLISH: {
            LOG(LL_DEBUG, ("%lu [%.*s] -> [%.*s]", c->id, (int) mm.topic.len,
                           mm.topic.ptr, (int) mm.data.len, mm.data.ptr));
            mg_call(c, MG_EV_MQTT_MSG, &mm);
            break;
          }
        }
        mg_call(c, MG_EV_MQTT_CMD, &mm);
        mg_iobuf_del(&c->recv, 0, mm.dgram.len);
      } else {
        break;
      }
    }
  }
  (void) ev_data;
  (void) fn_data;
}