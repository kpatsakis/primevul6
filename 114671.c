ndpi_l4_proto_info ndpi_get_l4_proto_info(struct ndpi_detection_module_struct *ndpi_struct,
					  u_int16_t ndpi_proto_id) {
  if(ndpi_proto_id < ndpi_struct->ndpi_num_supported_protocols) {
    u_int16_t idx = ndpi_struct->proto_defaults[ndpi_proto_id].protoIdx;
    NDPI_SELECTION_BITMASK_PROTOCOL_SIZE bm = ndpi_struct->callback_buffer[idx].ndpi_selection_bitmask;

    if(bm & NDPI_SELECTION_BITMASK_PROTOCOL_INT_TCP)
      return(ndpi_l4_proto_tcp_only);
    else if(bm & NDPI_SELECTION_BITMASK_PROTOCOL_INT_UDP)
      return(ndpi_l4_proto_udp_only);
    else if(bm & NDPI_SELECTION_BITMASK_PROTOCOL_INT_TCP_OR_UDP)
      return(ndpi_l4_proto_tcp_and_udp);
  }

  return(ndpi_l4_proto_unknown); /* default */
}