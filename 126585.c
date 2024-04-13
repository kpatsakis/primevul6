void mg_mqtt_disconnect(struct mg_connection *nc) {
  mg_mqtt_send_header(nc, MQTT_CMD_DISCONNECT, 0, 0);
}