u_int16_t ndpi_get_flow_masterprotocol(struct ndpi_detection_module_struct *ndpi_str, struct ndpi_flow_struct *flow) {
  return(flow->detected_protocol_stack[1]);
}