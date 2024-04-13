void mg_mqtt_ping(struct mg_connection *nc) {
  mg_mqtt_send_header(nc, MQTT_CMD_PINGREQ, 0, 0);
}