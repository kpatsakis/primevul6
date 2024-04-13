void mg_mqtt_pong(struct mg_connection *nc) {
  mg_mqtt_send_header(nc, MQTT_CMD_PINGRESP, 0, 0);
}