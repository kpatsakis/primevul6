u_int8_t ndpi_is_tor_flow(struct ndpi_detection_module_struct *ndpi_str, struct ndpi_flow_struct *flow) {
  struct ndpi_packet_struct *packet = &flow->packet;

  if(packet->tcp != NULL) {
    if(packet->iph) {
      if(flow->guessed_host_protocol_id == NDPI_PROTOCOL_TOR)
	return(1);
    }
  }

  return(0);
}