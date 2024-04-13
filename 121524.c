static void ndpi_int_ssh_add_connection(struct ndpi_detection_module_struct
					*ndpi_struct, struct ndpi_flow_struct *flow) {
  if(flow->extra_packets_func != NULL)
    return;

  flow->guessed_host_protocol_id = flow->guessed_protocol_id = NDPI_PROTOCOL_SSH;
  
  /* This is necessary to inform the core to call this dissector again */
  flow->check_extra_packets = 1;
  flow->max_extra_packets_to_check = 12;
  flow->extra_packets_func = search_ssh_again;
  
  ndpi_set_detected_protocol(ndpi_struct, flow, NDPI_PROTOCOL_SSH, NDPI_PROTOCOL_UNKNOWN);
}