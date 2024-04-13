ndpi_protocol ndpi_guess_undetected_protocol(struct ndpi_detection_module_struct *ndpi_str,
                                             struct ndpi_flow_struct *flow, u_int8_t proto,
                                             u_int32_t shost /* host byte order */, u_int16_t sport,
                                             u_int32_t dhost /* host byte order */, u_int16_t dport) {
  u_int32_t rc;
  struct in_addr addr;
  ndpi_protocol ret = {NDPI_PROTOCOL_UNKNOWN, NDPI_PROTOCOL_UNKNOWN, NDPI_PROTOCOL_CATEGORY_UNSPECIFIED};
  u_int8_t user_defined_proto;

  if((proto == IPPROTO_TCP) || (proto == IPPROTO_UDP)) {
    rc = ndpi_search_tcp_or_udp_raw(ndpi_str, flow, proto, shost, dhost, sport, dport);

    if(rc != NDPI_PROTOCOL_UNKNOWN) {
      if(flow && (proto == IPPROTO_UDP) &&
	 NDPI_COMPARE_PROTOCOL_TO_BITMASK(flow->excluded_protocol_bitmask, rc) && is_udp_guessable_protocol(rc))
	;
      else {
	ret.app_protocol = rc,
	  ret.master_protocol = ndpi_guess_protocol_id(ndpi_str, flow, proto, sport, dport, &user_defined_proto);

	if(ret.app_protocol == ret.master_protocol)
	  ret.master_protocol = NDPI_PROTOCOL_UNKNOWN;

	ret.category = ndpi_get_proto_category(ndpi_str, ret);
	return(ret);
      }
    }

    rc = ndpi_guess_protocol_id(ndpi_str, flow, proto, sport, dport, &user_defined_proto);
    if(rc != NDPI_PROTOCOL_UNKNOWN) {
      if(flow && (proto == IPPROTO_UDP) &&
	 NDPI_COMPARE_PROTOCOL_TO_BITMASK(flow->excluded_protocol_bitmask, rc) && is_udp_guessable_protocol(rc))
	;
      else {
	ret.app_protocol = rc;

	if(rc == NDPI_PROTOCOL_TLS)
	  goto check_guessed_skype;
	else {
	  ret.category = ndpi_get_proto_category(ndpi_str, ret);
	  return(ret);
	}
      }
    }

  check_guessed_skype:
    addr.s_addr = htonl(shost);
    if(ndpi_network_ptree_match(ndpi_str, &addr) == NDPI_PROTOCOL_SKYPE) {
      ret.app_protocol = NDPI_PROTOCOL_SKYPE;
    } else {
      addr.s_addr = htonl(dhost);
      if(ndpi_network_ptree_match(ndpi_str, &addr) == NDPI_PROTOCOL_SKYPE)
	ret.app_protocol = NDPI_PROTOCOL_SKYPE;
    }
  } else
    ret.app_protocol = ndpi_guess_protocol_id(ndpi_str, flow, proto, sport, dport, &user_defined_proto);

  ret.category = ndpi_get_proto_category(ndpi_str, ret);
  return(ret);
}