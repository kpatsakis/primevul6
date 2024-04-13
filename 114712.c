ndpi_protocol ndpi_detection_process_packet(struct ndpi_detection_module_struct *ndpi_str,
                                            struct ndpi_flow_struct *flow, const unsigned char *packet,
                                            const unsigned short packetlen, const u_int64_t current_tick_l,
                                            struct ndpi_id_struct *src, struct ndpi_id_struct *dst) {
  NDPI_SELECTION_BITMASK_PROTOCOL_SIZE ndpi_selection_packet;
  u_int32_t a;
  ndpi_protocol ret = {NDPI_PROTOCOL_UNKNOWN, NDPI_PROTOCOL_UNKNOWN, NDPI_PROTOCOL_CATEGORY_UNSPECIFIED};

  if(ndpi_str->ndpi_log_level >= NDPI_LOG_TRACE)
    NDPI_LOG(flow ? flow->detected_protocol_stack[0] : NDPI_PROTOCOL_UNKNOWN, ndpi_str, NDPI_LOG_TRACE,
	     "START packet processing\n");

  if(flow == NULL)
    return(ret);
  else
    ret.category = flow->category;

  flow->num_processed_pkts++;

  /* Init default */
  ret.master_protocol = flow->detected_protocol_stack[1], ret.app_protocol = flow->detected_protocol_stack[0];

  if(flow->server_id == NULL)
    flow->server_id = dst; /* Default */

  if(flow->detected_protocol_stack[0] != NDPI_PROTOCOL_UNKNOWN) {
    if(flow->check_extra_packets) {
      ndpi_process_extra_packet(ndpi_str, flow, packet, packetlen, current_tick_l, src, dst);
      /* Update in case of new match */
      ret.master_protocol = flow->detected_protocol_stack[1], ret.app_protocol = flow->detected_protocol_stack[0],
	ret.category = flow->category;
      goto invalidate_ptr;
    } else
      goto ret_protocols;
  }

  /* need at least 20 bytes for ip header */
  if(packetlen < 20) {
    /* reset protocol which is normally done in init_packet_header */
    ndpi_int_reset_packet_protocol(&flow->packet);
    goto invalidate_ptr;
  }

  flow->packet.tick_timestamp_l = current_tick_l;
  flow->packet.tick_timestamp = (u_int32_t)(current_tick_l / ndpi_str->ticks_per_second);

  /* parse packet */
  flow->packet.iph = (struct ndpi_iphdr *) packet;
  /* we are interested in ipv4 packet */

  if(ndpi_init_packet_header(ndpi_str, flow, packetlen) != 0)
    goto invalidate_ptr;

  /* detect traffic for tcp or udp only */
  flow->src = src, flow->dst = dst;

  ndpi_connection_tracking(ndpi_str, flow);

  /* build ndpi_selection packet bitmask */
  ndpi_selection_packet = NDPI_SELECTION_BITMASK_PROTOCOL_COMPLETE_TRAFFIC;
  if(flow->packet.iph != NULL)
    ndpi_selection_packet |= NDPI_SELECTION_BITMASK_PROTOCOL_IP | NDPI_SELECTION_BITMASK_PROTOCOL_IPV4_OR_IPV6;

  if(flow->packet.tcp != NULL)
    ndpi_selection_packet |=
      (NDPI_SELECTION_BITMASK_PROTOCOL_INT_TCP | NDPI_SELECTION_BITMASK_PROTOCOL_INT_TCP_OR_UDP);

  if(flow->packet.udp != NULL)
    ndpi_selection_packet |=
      (NDPI_SELECTION_BITMASK_PROTOCOL_INT_UDP | NDPI_SELECTION_BITMASK_PROTOCOL_INT_TCP_OR_UDP);

  if(flow->packet.payload_packet_len != 0)
    ndpi_selection_packet |= NDPI_SELECTION_BITMASK_PROTOCOL_HAS_PAYLOAD;

  if(flow->packet.tcp_retransmission == 0)
    ndpi_selection_packet |= NDPI_SELECTION_BITMASK_PROTOCOL_NO_TCP_RETRANSMISSION;

#ifdef NDPI_DETECTION_SUPPORT_IPV6
  if(flow->packet.iphv6 != NULL)
    ndpi_selection_packet |= NDPI_SELECTION_BITMASK_PROTOCOL_IPV6 | NDPI_SELECTION_BITMASK_PROTOCOL_IPV4_OR_IPV6;
#endif /* NDPI_DETECTION_SUPPORT_IPV6 */

  if((!flow->protocol_id_already_guessed) && (
#ifdef NDPI_DETECTION_SUPPORT_IPV6
					      flow->packet.iphv6 ||
#endif
					      flow->packet.iph)) {
    u_int16_t sport, dport;
    u_int8_t protocol;
    u_int8_t user_defined_proto;

    flow->protocol_id_already_guessed = 1;

#ifdef NDPI_DETECTION_SUPPORT_IPV6
    if(flow->packet.iphv6 != NULL) {
      protocol = flow->packet.iphv6->ip6_hdr.ip6_un1_nxt;
    } else
#endif
      {
	protocol = flow->packet.iph->protocol;
      }

    if(flow->packet.udp)
      sport = ntohs(flow->packet.udp->source), dport = ntohs(flow->packet.udp->dest);
    else if(flow->packet.tcp)
      sport = ntohs(flow->packet.tcp->source), dport = ntohs(flow->packet.tcp->dest);
    else
      sport = dport = 0;

    /* guess protocol */
    flow->guessed_protocol_id =
      (int16_t) ndpi_guess_protocol_id(ndpi_str, flow, protocol, sport, dport, &user_defined_proto);
    flow->guessed_host_protocol_id = ndpi_guess_host_protocol_id(ndpi_str, flow);

    if(ndpi_str->custom_categories.categories_loaded && flow->packet.iph) {
      ndpi_fill_ip_protocol_category(ndpi_str, flow->packet.iph->saddr, flow->packet.iph->daddr, &ret);
      flow->guessed_header_category = ret.category;
    } else
      flow->guessed_header_category = NDPI_PROTOCOL_CATEGORY_UNSPECIFIED;

    if(flow->guessed_protocol_id >= NDPI_MAX_SUPPORTED_PROTOCOLS) {
      /* This is a custom protocol and it has priority over everything else */
      ret.master_protocol = NDPI_PROTOCOL_UNKNOWN,
	ret.app_protocol = flow->guessed_protocol_id ? flow->guessed_protocol_id : flow->guessed_host_protocol_id;
      ndpi_fill_protocol_category(ndpi_str, flow, &ret);
      goto invalidate_ptr;
    }

    if(user_defined_proto && flow->guessed_protocol_id != NDPI_PROTOCOL_UNKNOWN) {
      if(flow->packet.iph) {
	if(flow->guessed_host_protocol_id != NDPI_PROTOCOL_UNKNOWN) {
	  u_int8_t protocol_was_guessed;

	  /* ret.master_protocol = flow->guessed_protocol_id , ret.app_protocol = flow->guessed_host_protocol_id; /\* ****** *\/ */
	  ret = ndpi_detection_giveup(ndpi_str, flow, 0, &protocol_was_guessed);
	}

	ndpi_fill_protocol_category(ndpi_str, flow, &ret);
	goto invalidate_ptr;
      }
    } else {
      /* guess host protocol */
      if(flow->packet.iph) {

	flow->guessed_host_protocol_id = ndpi_guess_host_protocol_id(ndpi_str, flow);

	/*
	  We could implement a shortcut here skipping dissectors for
	  protocols we have identified by other means such as with the IP

	  However we do NOT stop here and skip invoking the dissectors
	  because we want to dissect the flow (e.g. dissect the TLS)
	  and extract metadata.
	*/
#if SKIP_INVOKING_THE_DISSECTORS
	if(flow->guessed_host_protocol_id != NDPI_PROTOCOL_UNKNOWN) {
	  /*
	    We have identified a protocol using the IP address so
	    it is not worth to dissect the traffic as we already have
	    the solution
	  */
	  ret.master_protocol = flow->guessed_protocol_id, ret.app_protocol = flow->guessed_host_protocol_id;
	}
#endif
      }
    }
  }

  if(flow->guessed_host_protocol_id >= NDPI_MAX_SUPPORTED_PROTOCOLS) {
    /* This is a custom protocol and it has priority over everything else */
    ret.master_protocol = flow->guessed_protocol_id, ret.app_protocol = flow->guessed_host_protocol_id;

    ndpi_check_flow_func(ndpi_str, flow, &ndpi_selection_packet);
    ndpi_fill_protocol_category(ndpi_str, flow, &ret);
    goto invalidate_ptr;
  }

  ndpi_check_flow_func(ndpi_str, flow, &ndpi_selection_packet);

  a = flow->packet.detected_protocol_stack[0];
  if(NDPI_COMPARE_PROTOCOL_TO_BITMASK(ndpi_str->detection_bitmask, a) == 0)
    a = NDPI_PROTOCOL_UNKNOWN;

  if(a != NDPI_PROTOCOL_UNKNOWN) {
    int i;

    for (i = 0; i < sizeof(flow->host_server_name); i++) {
      if(flow->host_server_name[i] != '\0')
	flow->host_server_name[i] = tolower(flow->host_server_name[i]);
      else {
	flow->host_server_name[i] = '\0';
	break;
      }
    }
  }

 ret_protocols:
  if(flow->detected_protocol_stack[1] != NDPI_PROTOCOL_UNKNOWN) {
    ret.master_protocol = flow->detected_protocol_stack[1], ret.app_protocol = flow->detected_protocol_stack[0];

    if(ret.app_protocol == ret.master_protocol)
      ret.master_protocol = NDPI_PROTOCOL_UNKNOWN;
  } else
    ret.app_protocol = flow->detected_protocol_stack[0];

  /* Don't overwrite the category if already set */
  if((flow->category == NDPI_PROTOCOL_CATEGORY_UNSPECIFIED) && (ret.app_protocol != NDPI_PROTOCOL_UNKNOWN))
    ndpi_fill_protocol_category(ndpi_str, flow, &ret);
  else
    ret.category = flow->category;

  if((flow->num_processed_pkts == 1) && (ret.master_protocol == NDPI_PROTOCOL_UNKNOWN) &&
     (ret.app_protocol == NDPI_PROTOCOL_UNKNOWN) && flow->packet.tcp && (flow->packet.tcp->syn == 0) &&
     (flow->guessed_protocol_id == 0)) {
    u_int8_t protocol_was_guessed;

    /*
      This is a TCP flow
      - whose first packet is NOT a SYN
      - no protocol has been detected

      We don't see how future packets can match anything
      hence we giveup here
    */
    ret = ndpi_detection_giveup(ndpi_str, flow, 0, &protocol_was_guessed);
  }

  if((ret.master_protocol == NDPI_PROTOCOL_UNKNOWN) && (ret.app_protocol != NDPI_PROTOCOL_UNKNOWN) &&
     (flow->guessed_host_protocol_id != NDPI_PROTOCOL_UNKNOWN)) {
    ret.master_protocol = ret.app_protocol;
    ret.app_protocol = flow->guessed_host_protocol_id;
  }

  if((!flow->risk_checked) && (ret.master_protocol != NDPI_PROTOCOL_UNKNOWN)) {
    ndpi_default_ports_tree_node_t *found;
    u_int16_t *default_ports, sport, dport;

    if(flow->packet.udp)
      found = ndpi_get_guessed_protocol_id(ndpi_str, IPPROTO_UDP,
					   sport = ntohs(flow->packet.udp->source),
					   dport = ntohs(flow->packet.udp->dest)),
	default_ports = ndpi_str->proto_defaults[ret.master_protocol].udp_default_ports;
    else if(flow->packet.tcp)
      found = ndpi_get_guessed_protocol_id(ndpi_str, IPPROTO_TCP,
					   sport = ntohs(flow->packet.tcp->source),
					   dport = ntohs(flow->packet.tcp->dest)),
	default_ports = ndpi_str->proto_defaults[ret.master_protocol].tcp_default_ports;
    else
      found = NULL, default_ports = NULL;

    if(found
       && (found->proto->protoId != NDPI_PROTOCOL_UNKNOWN)
       && (found->proto->protoId != ret.master_protocol)) {
      // printf("******** %u / %u\n", found->proto->protoId, ret.master_protocol);
      NDPI_SET_BIT(flow->risk, NDPI_KNOWN_PROTOCOL_ON_NON_STANDARD_PORT);
    } else if(default_ports && (default_ports[0] != 0)) {
      u_int8_t found = 0, i;

      for(i=0; (i<MAX_DEFAULT_PORTS) && (default_ports[i] != 0); i++) {
	if((default_ports[i] == sport) || (default_ports[i] == dport)) {
	  found = 1;
	  break;
	}
      } /* for */

      if(!found) {
	// printf("******** Invalid default port\n");
	NDPI_SET_BIT(flow->risk, NDPI_KNOWN_PROTOCOL_ON_NON_STANDARD_PORT);
      }
    }

    flow->risk_checked = 1;
  }

  ndpi_reconcile_protocols(ndpi_str, flow, &ret);

 invalidate_ptr:
  /*
    Invalidate packet memory to avoid accessing the pointers below
    when the packet is no longer accessible
  */
  flow->packet.iph = NULL, flow->packet.tcp = NULL, flow->packet.udp = NULL, flow->packet.payload = NULL;
  ndpi_reset_packet_line_info(&flow->packet);

  return(ret);
}