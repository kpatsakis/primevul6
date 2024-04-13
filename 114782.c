u_int16_t ndpi_guess_protocol_id(struct ndpi_detection_module_struct *ndpi_str, struct ndpi_flow_struct *flow,
                                 u_int8_t proto, u_int16_t sport, u_int16_t dport, u_int8_t *user_defined_proto) {
  *user_defined_proto = 0; /* Default */

  if(sport && dport) {
    ndpi_default_ports_tree_node_t *found = ndpi_get_guessed_protocol_id(ndpi_str, proto, sport, dport);

    if(found != NULL) {
      u_int16_t guessed_proto = found->proto->protoId;

      /* We need to check if the guessed protocol isn't excluded by nDPI */
      if(flow && (proto == IPPROTO_UDP) &&
	 NDPI_COMPARE_PROTOCOL_TO_BITMASK(flow->excluded_protocol_bitmask, guessed_proto) &&
	 is_udp_guessable_protocol(guessed_proto))
	return(NDPI_PROTOCOL_UNKNOWN);
      else {
	*user_defined_proto = found->customUserProto;
	return(guessed_proto);
      }
    }
  } else {
    /* No TCP/UDP */

    switch (proto) {
    case NDPI_IPSEC_PROTOCOL_ESP:
    case NDPI_IPSEC_PROTOCOL_AH:
      return(NDPI_PROTOCOL_IP_IPSEC);
      break;
    case NDPI_GRE_PROTOCOL_TYPE:
      return(NDPI_PROTOCOL_IP_GRE);
      break;
    case NDPI_ICMP_PROTOCOL_TYPE:
      return(NDPI_PROTOCOL_IP_ICMP);
      break;
    case NDPI_IGMP_PROTOCOL_TYPE:
      return(NDPI_PROTOCOL_IP_IGMP);
      break;
    case NDPI_EGP_PROTOCOL_TYPE:
      return(NDPI_PROTOCOL_IP_EGP);
      break;
    case NDPI_SCTP_PROTOCOL_TYPE:
      return(NDPI_PROTOCOL_IP_SCTP);
      break;
    case NDPI_OSPF_PROTOCOL_TYPE:
      return(NDPI_PROTOCOL_IP_OSPF);
      break;
    case NDPI_IPIP_PROTOCOL_TYPE:
      return(NDPI_PROTOCOL_IP_IP_IN_IP);
      break;
    case NDPI_ICMPV6_PROTOCOL_TYPE:
      return(NDPI_PROTOCOL_IP_ICMPV6);
      break;
    case 112:
      return(NDPI_PROTOCOL_IP_VRRP);
      break;
    }
  }

  return(NDPI_PROTOCOL_UNKNOWN);
}