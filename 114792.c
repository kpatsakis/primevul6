void ndpi_int_change_protocol(struct ndpi_detection_module_struct *ndpi_str, struct ndpi_flow_struct *flow,
                              u_int16_t upper_detected_protocol, u_int16_t lower_detected_protocol) {
  if((upper_detected_protocol == NDPI_PROTOCOL_UNKNOWN) && (lower_detected_protocol != NDPI_PROTOCOL_UNKNOWN))
    upper_detected_protocol = lower_detected_protocol;

  if(upper_detected_protocol == lower_detected_protocol)
    lower_detected_protocol = NDPI_PROTOCOL_UNKNOWN;

  if((upper_detected_protocol != NDPI_PROTOCOL_UNKNOWN) && (lower_detected_protocol == NDPI_PROTOCOL_UNKNOWN)) {
    if((flow->guessed_host_protocol_id != NDPI_PROTOCOL_UNKNOWN) &&
       (upper_detected_protocol != flow->guessed_host_protocol_id)) {
      if(ndpi_str->proto_defaults[upper_detected_protocol].can_have_a_subprotocol) {
	lower_detected_protocol = upper_detected_protocol;
	upper_detected_protocol = flow->guessed_host_protocol_id;
      }
    }
  }

  ndpi_int_change_flow_protocol(ndpi_str, flow, upper_detected_protocol, lower_detected_protocol);
  ndpi_int_change_packet_protocol(ndpi_str, flow, upper_detected_protocol, lower_detected_protocol);
}