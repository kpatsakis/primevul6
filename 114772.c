void ndpi_int_change_packet_protocol(struct ndpi_detection_module_struct *ndpi_str, struct ndpi_flow_struct *flow,
                                     u_int16_t upper_detected_protocol, u_int16_t lower_detected_protocol) {
  struct ndpi_packet_struct *packet = &flow->packet;
  /* NOTE: everything below is identically to change_flow_protocol
   *        except flow->packet If you want to change something here,
   *        don't! Change it for the flow function and apply it here
   *        as well */

  if(!packet)
    return;

  packet->detected_protocol_stack[0] = upper_detected_protocol,
    packet->detected_protocol_stack[1] = lower_detected_protocol;
}