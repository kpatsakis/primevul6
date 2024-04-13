u_int16_t ndpi_guess_host_protocol_id(struct ndpi_detection_module_struct *ndpi_str,
				      struct ndpi_flow_struct *flow) {
  u_int16_t ret = NDPI_PROTOCOL_UNKNOWN;

  if(flow->packet.iph) {
    struct in_addr addr;
    u_int16_t sport, dport;

    addr.s_addr = flow->packet.iph->saddr;

    if((flow->l4_proto == IPPROTO_TCP) && flow->packet.tcp)
      sport = flow->packet.tcp->source, dport = flow->packet.tcp->dest;
    else if((flow->l4_proto == IPPROTO_UDP) && flow->packet.udp)
      sport = flow->packet.udp->source, dport = flow->packet.udp->dest;
    else
      sport = dport = 0;

    /* guess host protocol */
    ret = ndpi_network_port_ptree_match(ndpi_str, &addr, sport);

    if(ret == NDPI_PROTOCOL_UNKNOWN) {
      addr.s_addr = flow->packet.iph->daddr;
      ret = ndpi_network_port_ptree_match(ndpi_str, &addr, dport);
    }
  }

  return(ret);
}