static void test_mqtt(void) {
  char buf[50] = {0};
  struct mg_mgr mgr;
  struct mg_str topic = mg_str("x/f12"), data = mg_str("hi");
  struct mg_connection *c;
  struct mg_mqtt_opts opts;
  // const char *url = "mqtt://mqtt.eclipse.org:1883";
  const char *url = "mqtt://broker.hivemq.com:1883";
  int i;
  mg_mgr_init(&mgr);

  {
    uint8_t bad[] = " \xff\xff\xff\xff ";
    struct mg_mqtt_message mm;
    mg_mqtt_parse(bad, sizeof(bad), &mm);
  }

  // Connect with empty client ID
  c = mg_mqtt_connect(&mgr, url, NULL, mqtt_cb, buf);
  for (i = 0; i < 200 && buf[0] == 0; i++) mg_mgr_poll(&mgr, 10);
  ASSERT(buf[0] == 'X');
  mg_mqtt_sub(c, &topic, 1);
  mg_mqtt_pub(c, &topic, &data, 1, false);
  for (i = 0; i < 300 && buf[1] == 0; i++) mg_mgr_poll(&mgr, 10);
  // LOG(LL_INFO, ("[%s]", buf));
  ASSERT(strcmp(buf, "Xx/f12/hi") == 0);

  // Set params
  memset(buf, 0, sizeof(buf));
  memset(&opts, 0, sizeof(opts));
  opts.clean = true;
  opts.will_qos = 1;
  opts.will_retain = true;
  opts.keepalive = 20;
  opts.client_id = mg_str("mg_client");
  opts.will_topic = mg_str("mg_will_topic");
  opts.will_message = mg_str("mg_will_messsage");
  c = mg_mqtt_connect(&mgr, url, &opts, mqtt_cb, buf);
  for (i = 0; i < 300 && buf[0] == 0; i++) mg_mgr_poll(&mgr, 10);
  ASSERT(buf[0] == 'X');
  mg_mqtt_sub(c, &topic, 1);
  mg_mqtt_pub(c, &topic, &data, 1, false);
  for (i = 0; i < 500 && buf[1] == 0; i++) mg_mgr_poll(&mgr, 10);
  ASSERT(strcmp(buf, "Xx/f12/hi") == 0);

  mg_mgr_free(&mgr);
  ASSERT(mgr.conns == NULL);
}