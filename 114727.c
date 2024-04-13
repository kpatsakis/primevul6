static u_int16_t ndpi_automa_match_string_subprotocol(struct ndpi_detection_module_struct *ndpi_str,
                                                      struct ndpi_flow_struct *flow, char *string_to_match,
                                                      u_int string_to_match_len, u_int16_t master_protocol_id,
                                                      ndpi_protocol_match_result *ret_match, u_int8_t is_host_match) {
  int matching_protocol_id;
  struct ndpi_packet_struct *packet = &flow->packet;

  matching_protocol_id =
    ndpi_match_string_subprotocol(ndpi_str, string_to_match, string_to_match_len, ret_match, is_host_match);

#ifdef DEBUG
  {
    char m[256];
    int len = ndpi_min(sizeof(m), string_to_match_len);

    strncpy(m, string_to_match, len);
    m[len] = '\0';

    NDPI_LOG_DBG2(ndpi_str, "[NDPI] ndpi_match_host_subprotocol(%s): %s\n", m,
		  ndpi_str->proto_defaults[matching_protocol_id].protoName);
  }
#endif

  if((matching_protocol_id != NDPI_PROTOCOL_UNKNOWN) &&
     (!ndpi_is_more_generic_protocol(packet->detected_protocol_stack[0], matching_protocol_id))) {
    /* Move the protocol on slot 0 down one position */
    packet->detected_protocol_stack[1] = master_protocol_id,
      packet->detected_protocol_stack[0] = matching_protocol_id;

    flow->detected_protocol_stack[0] = packet->detected_protocol_stack[0],
      flow->detected_protocol_stack[1] = packet->detected_protocol_stack[1];

    if(flow->category == NDPI_PROTOCOL_CATEGORY_UNSPECIFIED)
      flow->category = ret_match->protocol_category;

    return(packet->detected_protocol_stack[0]);
  }

#ifdef DEBUG
  string_to_match[string_to_match_len] = '\0';
  NDPI_LOG_DBG2(ndpi_str, "[NTOP] Unable to find a match for '%s'\n", string_to_match);
#endif

  ret_match->protocol_id = NDPI_PROTOCOL_UNKNOWN, ret_match->protocol_category = NDPI_PROTOCOL_CATEGORY_UNSPECIFIED,
    ret_match->protocol_breed = NDPI_PROTOCOL_UNRATED;

  return(NDPI_PROTOCOL_UNKNOWN);
}