void check_ndpi_tcp_flow_func(struct ndpi_detection_module_struct *ndpi_str,
			      struct ndpi_flow_struct *flow,
                              NDPI_SELECTION_BITMASK_PROTOCOL_SIZE *ndpi_selection_packet) {
  void *func = NULL;
  u_int32_t a;
  u_int16_t proto_index = ndpi_str->proto_defaults[flow->guessed_protocol_id].protoIdx;
  int16_t proto_id = ndpi_str->proto_defaults[flow->guessed_protocol_id].protoId;
  NDPI_PROTOCOL_BITMASK detection_bitmask;

  NDPI_SAVE_AS_BITMASK(detection_bitmask, flow->packet.detected_protocol_stack[0]);

  if(flow->packet.payload_packet_len != 0) {
    if((proto_id != NDPI_PROTOCOL_UNKNOWN) &&
       NDPI_BITMASK_COMPARE(flow->excluded_protocol_bitmask,
			    ndpi_str->callback_buffer[proto_index].excluded_protocol_bitmask) == 0 &&
       NDPI_BITMASK_COMPARE(ndpi_str->callback_buffer[proto_index].detection_bitmask, detection_bitmask) != 0 &&
       (ndpi_str->callback_buffer[proto_index].ndpi_selection_bitmask & *ndpi_selection_packet) ==
       ndpi_str->callback_buffer[proto_index].ndpi_selection_bitmask) {
      if((flow->guessed_protocol_id != NDPI_PROTOCOL_UNKNOWN) &&
	 (ndpi_str->proto_defaults[flow->guessed_protocol_id].func != NULL))
	ndpi_str->proto_defaults[flow->guessed_protocol_id].func(ndpi_str, flow),
	  func = ndpi_str->proto_defaults[flow->guessed_protocol_id].func;
    }

    if(flow->detected_protocol_stack[0] == NDPI_PROTOCOL_UNKNOWN) {
      for (a = 0; a < ndpi_str->callback_buffer_size_tcp_payload; a++) {
	if((func != ndpi_str->callback_buffer_tcp_payload[a].func) &&
	   (ndpi_str->callback_buffer_tcp_payload[a].ndpi_selection_bitmask & *ndpi_selection_packet) ==
	   ndpi_str->callback_buffer_tcp_payload[a].ndpi_selection_bitmask &&
	   NDPI_BITMASK_COMPARE(flow->excluded_protocol_bitmask,
				ndpi_str->callback_buffer_tcp_payload[a].excluded_protocol_bitmask) == 0 &&
	   NDPI_BITMASK_COMPARE(ndpi_str->callback_buffer_tcp_payload[a].detection_bitmask,
				detection_bitmask) != 0) {
	  ndpi_str->callback_buffer_tcp_payload[a].func(ndpi_str, flow);

	  if(flow->detected_protocol_stack[0] != NDPI_PROTOCOL_UNKNOWN)
	    break; /* Stop after detecting the first protocol */
	}
      }
    }
  } else {
    /* no payload */
    if((proto_id != NDPI_PROTOCOL_UNKNOWN) &&
       NDPI_BITMASK_COMPARE(flow->excluded_protocol_bitmask,
			    ndpi_str->callback_buffer[proto_index].excluded_protocol_bitmask) == 0 &&
       NDPI_BITMASK_COMPARE(ndpi_str->callback_buffer[proto_index].detection_bitmask, detection_bitmask) != 0 &&
       (ndpi_str->callback_buffer[proto_index].ndpi_selection_bitmask & *ndpi_selection_packet) ==
       ndpi_str->callback_buffer[proto_index].ndpi_selection_bitmask) {
      if((flow->guessed_protocol_id != NDPI_PROTOCOL_UNKNOWN) &&
	 (ndpi_str->proto_defaults[flow->guessed_protocol_id].func != NULL) &&
	 ((ndpi_str->callback_buffer[flow->guessed_protocol_id].ndpi_selection_bitmask &
	   NDPI_SELECTION_BITMASK_PROTOCOL_HAS_PAYLOAD) == 0))
	ndpi_str->proto_defaults[flow->guessed_protocol_id].func(ndpi_str, flow),
	  func = ndpi_str->proto_defaults[flow->guessed_protocol_id].func;
    }

    for (a = 0; a < ndpi_str->callback_buffer_size_tcp_no_payload; a++) {
      if((func != ndpi_str->callback_buffer_tcp_payload[a].func) &&
	 (ndpi_str->callback_buffer_tcp_no_payload[a].ndpi_selection_bitmask & *ndpi_selection_packet) ==
	 ndpi_str->callback_buffer_tcp_no_payload[a].ndpi_selection_bitmask &&
	 NDPI_BITMASK_COMPARE(flow->excluded_protocol_bitmask,
			      ndpi_str->callback_buffer_tcp_no_payload[a].excluded_protocol_bitmask) == 0 &&
	 NDPI_BITMASK_COMPARE(ndpi_str->callback_buffer_tcp_no_payload[a].detection_bitmask,
			      detection_bitmask) != 0) {
	ndpi_str->callback_buffer_tcp_no_payload[a].func(ndpi_str, flow);

	if(flow->detected_protocol_stack[0] != NDPI_PROTOCOL_UNKNOWN)
	  break; /* Stop after detecting the first protocol */
      }
    }
  }
}