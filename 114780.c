void ndpi_int_change_flow_protocol(struct ndpi_detection_module_struct *ndpi_str, struct ndpi_flow_struct *flow,
                                   u_int16_t upper_detected_protocol, u_int16_t lower_detected_protocol) {
  if(!flow)
    return;

  flow->detected_protocol_stack[0] = upper_detected_protocol,
    flow->detected_protocol_stack[1] = lower_detected_protocol;
}