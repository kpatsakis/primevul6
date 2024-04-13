void ndpi_apply_flow_protocol_to_packet(struct ndpi_flow_struct *flow, struct ndpi_packet_struct *packet) {
  memcpy(&packet->detected_protocol_stack, &flow->detected_protocol_stack, sizeof(packet->detected_protocol_stack));
  memcpy(&packet->protocol_stack_info, &flow->protocol_stack_info, sizeof(packet->protocol_stack_info));
}