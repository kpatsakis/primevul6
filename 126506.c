static void mqtt_cb(struct mg_connection *c, int ev, void *evd, void *fnd) {
  char *buf = (char *) fnd;
  if (ev == MG_EV_MQTT_OPEN) {
    buf[0] = *(int *) evd == 0 ? 'X' : 'Y';
  } else if (ev == MG_EV_MQTT_MSG) {
    struct mg_mqtt_message *mm = (struct mg_mqtt_message *) evd;
    sprintf(buf + 1, "%.*s/%.*s", (int) mm->topic.len, mm->topic.ptr,
            (int) mm->data.len, mm->data.ptr);
  }
  (void) c;
}