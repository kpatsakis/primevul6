void ndpi_int_reset_protocol(struct ndpi_flow_struct *flow) {
  if(flow) {
    int a;

    for (a = 0; a < NDPI_PROTOCOL_SIZE; a++)
      flow->detected_protocol_stack[a] = NDPI_PROTOCOL_UNKNOWN;
  }
}