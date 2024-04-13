ndpi_protocol ndpi_detection_giveup(struct ndpi_detection_module_struct *ndpi_str, struct ndpi_flow_struct *flow,
                                    u_int8_t enable_guess, u_int8_t *protocol_was_guessed) {
  ndpi_protocol ret = {NDPI_PROTOCOL_UNKNOWN, NDPI_PROTOCOL_UNKNOWN, NDPI_PROTOCOL_CATEGORY_UNSPECIFIED};

  *protocol_was_guessed = 0;

  if(flow == NULL)
    return(ret);

  /* Init defaults */
  ret.master_protocol = flow->detected_protocol_stack[1], ret.app_protocol = flow->detected_protocol_stack[0];
  ret.category = flow->category;

  /* Ensure that we don't change our mind if detection is already complete */
  if((ret.master_protocol != NDPI_PROTOCOL_UNKNOWN) && (ret.app_protocol != NDPI_PROTOCOL_UNKNOWN))
    return(ret);

  /* TODO: add the remaining stage_XXXX protocols */
  if(flow->detected_protocol_stack[0] == NDPI_PROTOCOL_UNKNOWN) {
    u_int16_t guessed_protocol_id = NDPI_PROTOCOL_UNKNOWN, guessed_host_protocol_id = NDPI_PROTOCOL_UNKNOWN;

    if(flow->guessed_protocol_id == NDPI_PROTOCOL_STUN)
      goto check_stun_export;
    else if((flow->guessed_protocol_id == NDPI_PROTOCOL_HANGOUT_DUO) ||
	    (flow->guessed_protocol_id == NDPI_PROTOCOL_MESSENGER) ||
	    (flow->guessed_protocol_id == NDPI_PROTOCOL_WHATSAPP_CALL)) {
      *protocol_was_guessed = 1;
      ndpi_set_detected_protocol(ndpi_str, flow, flow->guessed_protocol_id, NDPI_PROTOCOL_UNKNOWN);
    }
    else if((flow->l4.tcp.tls.hello_processed == 1) &&
	    (flow->protos.stun_ssl.ssl.client_requested_server_name[0] != '\0')) {
      *protocol_was_guessed = 1;
      ndpi_set_detected_protocol(ndpi_str, flow, NDPI_PROTOCOL_TLS, NDPI_PROTOCOL_UNKNOWN);
    } else if(enable_guess) {
      if((flow->guessed_protocol_id == NDPI_PROTOCOL_UNKNOWN) && (flow->packet.l4_protocol == IPPROTO_TCP) &&
	 flow->l4.tcp.tls.hello_processed)
	flow->guessed_protocol_id = NDPI_PROTOCOL_TLS;

      guessed_protocol_id = flow->guessed_protocol_id, guessed_host_protocol_id = flow->guessed_host_protocol_id;

      if((guessed_host_protocol_id != NDPI_PROTOCOL_UNKNOWN) &&
	 ((flow->packet.l4_protocol == IPPROTO_UDP) &&
	  NDPI_ISSET(&flow->excluded_protocol_bitmask, guessed_host_protocol_id) &&
	  is_udp_guessable_protocol(guessed_host_protocol_id)))
	flow->guessed_host_protocol_id = guessed_host_protocol_id = NDPI_PROTOCOL_UNKNOWN;

      /* Ignore guessed protocol if they have been discarded */
      if((guessed_protocol_id != NDPI_PROTOCOL_UNKNOWN)
	 // && (guessed_host_protocol_id == NDPI_PROTOCOL_UNKNOWN)
	 && (flow->packet.l4_protocol == IPPROTO_UDP) &&
	 NDPI_ISSET(&flow->excluded_protocol_bitmask, guessed_protocol_id) &&
	 is_udp_guessable_protocol(guessed_protocol_id))
	flow->guessed_protocol_id = guessed_protocol_id = NDPI_PROTOCOL_UNKNOWN;

      if((guessed_protocol_id != NDPI_PROTOCOL_UNKNOWN) || (guessed_host_protocol_id != NDPI_PROTOCOL_UNKNOWN)) {
	if((guessed_protocol_id == 0) && (flow->protos.stun_ssl.stun.num_binding_requests > 0) &&
	   (flow->protos.stun_ssl.stun.num_processed_pkts > 0))
	  guessed_protocol_id = NDPI_PROTOCOL_STUN;

	if(flow->host_server_name[0] != '\0') {
	  ndpi_protocol_match_result ret_match;

	  memset(&ret_match, 0, sizeof(ret_match));

	  ndpi_match_host_subprotocol(ndpi_str, flow, (char *) flow->host_server_name,
				      strlen((const char *) flow->host_server_name), &ret_match,
				      NDPI_PROTOCOL_DNS);

	  if(ret_match.protocol_id != NDPI_PROTOCOL_UNKNOWN)
	    guessed_host_protocol_id = ret_match.protocol_id;
	}

	*protocol_was_guessed = 1;
	ndpi_int_change_protocol(ndpi_str, flow, guessed_host_protocol_id, guessed_protocol_id);
      }
    }
  } else if(enable_guess) {
    if(flow->guessed_protocol_id != NDPI_PROTOCOL_UNKNOWN) {
      *protocol_was_guessed = 1;
      flow->detected_protocol_stack[1] = flow->guessed_protocol_id;
    }

    if(flow->guessed_host_protocol_id != NDPI_PROTOCOL_UNKNOWN) {
      *protocol_was_guessed = 1;
      flow->detected_protocol_stack[0] = flow->guessed_host_protocol_id;
    }

    if(flow->detected_protocol_stack[1] == flow->detected_protocol_stack[0]) {
      *protocol_was_guessed = 1;
      flow->detected_protocol_stack[1] = flow->guessed_host_protocol_id;
    }
  }

  if((flow->detected_protocol_stack[0] == NDPI_PROTOCOL_UNKNOWN) &&
     (flow->guessed_protocol_id == NDPI_PROTOCOL_STUN)) {
  check_stun_export:
    if(flow->protos.stun_ssl.stun.num_processed_pkts || flow->protos.stun_ssl.stun.num_udp_pkts) {
      // if(/* (flow->protos.stun_ssl.stun.num_processed_pkts >= NDPI_MIN_NUM_STUN_DETECTION) */
      *protocol_was_guessed = 1;
      ndpi_set_detected_protocol(ndpi_str, flow, flow->guessed_host_protocol_id, NDPI_PROTOCOL_STUN);
    }
  }

  ret.master_protocol = flow->detected_protocol_stack[1], ret.app_protocol = flow->detected_protocol_stack[0];

  if(ret.master_protocol == NDPI_PROTOCOL_STUN) {
    if(ret.app_protocol == NDPI_PROTOCOL_FACEBOOK)
      ret.app_protocol = NDPI_PROTOCOL_MESSENGER;
    else if(ret.app_protocol == NDPI_PROTOCOL_GOOGLE) {
      /*
	As Google has recently introduced Duo,
	we need to distinguish between it and hangout
	thing that should be handled by the STUN dissector
      */
      ret.app_protocol = NDPI_PROTOCOL_HANGOUT_DUO;
    }
  }

  if(ret.app_protocol != NDPI_PROTOCOL_UNKNOWN) {
    *protocol_was_guessed = 1;
    ndpi_fill_protocol_category(ndpi_str, flow, &ret);
  }

  return(ret);
}