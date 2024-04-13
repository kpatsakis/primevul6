uint16_t mg_ntohs(uint16_t net) {
  uint8_t data[2] = {0, 0};
  memcpy(&data, &net, sizeof(data));
  return (uint16_t) ((uint16_t) data[1] | (((uint16_t) data[0]) << 8));
}