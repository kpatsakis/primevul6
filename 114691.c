void ndpi_int_reset_packet_protocol(struct ndpi_packet_struct *packet) {
  int a;

  for (a = 0; a < NDPI_PROTOCOL_SIZE; a++)
    packet->detected_protocol_stack[a] = NDPI_PROTOCOL_UNKNOWN;
}