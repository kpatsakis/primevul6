void ndpi_set_detected_protocol(struct ndpi_detection_module_struct *ndpi_str, struct ndpi_flow_struct *flow,
                                u_int16_t upper_detected_protocol, u_int16_t lower_detected_protocol) {
  struct ndpi_id_struct *src = flow->src, *dst = flow->dst;

  ndpi_int_change_protocol(ndpi_str, flow, upper_detected_protocol, lower_detected_protocol);

  if(src != NULL) {
    NDPI_ADD_PROTOCOL_TO_BITMASK(src->detected_protocol_bitmask, upper_detected_protocol);

    if(lower_detected_protocol != NDPI_PROTOCOL_UNKNOWN)
      NDPI_ADD_PROTOCOL_TO_BITMASK(src->detected_protocol_bitmask, lower_detected_protocol);
  }

  if(dst != NULL) {
    NDPI_ADD_PROTOCOL_TO_BITMASK(dst->detected_protocol_bitmask, upper_detected_protocol);

    if(lower_detected_protocol != NDPI_PROTOCOL_UNKNOWN)
      NDPI_ADD_PROTOCOL_TO_BITMASK(dst->detected_protocol_bitmask, lower_detected_protocol);
  }
}