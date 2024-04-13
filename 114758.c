void ndpi_check_flow_func(struct ndpi_detection_module_struct *ndpi_str, struct ndpi_flow_struct *flow,
                          NDPI_SELECTION_BITMASK_PROTOCOL_SIZE *ndpi_selection_packet) {
  if(flow->packet.tcp != NULL)
    check_ndpi_tcp_flow_func(ndpi_str, flow, ndpi_selection_packet);
  else if(flow->packet.udp != NULL)
    check_ndpi_udp_flow_func(ndpi_str, flow, ndpi_selection_packet);
  else
    check_ndpi_other_flow_func(ndpi_str, flow, ndpi_selection_packet);
}