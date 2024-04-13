static int ndpi_init_packet_header(struct ndpi_detection_module_struct *ndpi_str,
				   struct ndpi_flow_struct *flow,
                                   unsigned short packetlen) {
  const struct ndpi_iphdr *decaps_iph = NULL;
  u_int16_t l3len;
  u_int16_t l4len;
  const u_int8_t *l4ptr;
  u_int8_t l4protocol;
  u_int8_t l4_result;

  if(!flow)
    return(1);

  /* reset payload_packet_len, will be set if ipv4 tcp or udp */
  flow->packet.payload_packet_len = 0;
  flow->packet.l4_packet_len = 0;
  flow->packet.l3_packet_len = packetlen;

  flow->packet.tcp = NULL, flow->packet.udp = NULL;
  flow->packet.generic_l4_ptr = NULL;
#ifdef NDPI_DETECTION_SUPPORT_IPV6
  flow->packet.iphv6 = NULL;
#endif /* NDPI_DETECTION_SUPPORT_IPV6 */

  ndpi_apply_flow_protocol_to_packet(flow, &flow->packet);

  l3len = flow->packet.l3_packet_len;

#ifdef NDPI_DETECTION_SUPPORT_IPV6
  if(flow->packet.iph != NULL) {
#endif /* NDPI_DETECTION_SUPPORT_IPV6 */

    decaps_iph = flow->packet.iph;

#ifdef NDPI_DETECTION_SUPPORT_IPV6
  }
#endif /* NDPI_DETECTION_SUPPORT_IPV6 */

  if(decaps_iph && decaps_iph->version == IPVERSION && decaps_iph->ihl >= 5) {
    NDPI_LOG_DBG2(ndpi_str, "ipv4 header\n");
  }
#ifdef NDPI_DETECTION_SUPPORT_IPV6
  else if(decaps_iph && decaps_iph->version == 6 && l3len >= sizeof(struct ndpi_ipv6hdr) &&
	  (ndpi_str->ip_version_limit & NDPI_DETECTION_ONLY_IPV4) == 0) {
    NDPI_LOG_DBG2(ndpi_str, "ipv6 header\n");
    flow->packet.iphv6 = (struct ndpi_ipv6hdr *) flow->packet.iph;
    flow->packet.iph = NULL;
  }
#endif
  else {
    flow->packet.iph = NULL;
    return(1);
  }

  /* needed:
   *  - unfragmented packets
   *  - ip header <= packet len
   *  - ip total length >= packet len
   */

  l4ptr = NULL;
  l4len = 0;
  l4protocol = 0;

  l4_result =
    ndpi_detection_get_l4_internal(ndpi_str, (const u_int8_t *) decaps_iph, l3len, &l4ptr, &l4len, &l4protocol, 0);

  if(l4_result != 0) {
    return(1);
  }

  flow->packet.l4_protocol = l4protocol;
  flow->packet.l4_packet_len = l4len;
  flow->l4_proto = l4protocol;

  /* tcp / udp detection */
  if(l4protocol == IPPROTO_TCP && flow->packet.l4_packet_len >= 20 /* min size of tcp */) {
    /* tcp */
    flow->packet.tcp = (struct ndpi_tcphdr *) l4ptr;
    if(flow->packet.l4_packet_len >= flow->packet.tcp->doff * 4) {
      flow->packet.payload_packet_len = flow->packet.l4_packet_len - flow->packet.tcp->doff * 4;
      flow->packet.actual_payload_len = flow->packet.payload_packet_len;
      flow->packet.payload = ((u_int8_t *) flow->packet.tcp) + (flow->packet.tcp->doff * 4);

      /* check for new tcp syn packets, here
       * idea: reset detection state if a connection is unknown
       */
      if(flow->packet.tcp->syn != 0 && flow->packet.tcp->ack == 0 && flow->init_finished != 0 &&
	 flow->detected_protocol_stack[0] == NDPI_PROTOCOL_UNKNOWN) {
	u_int8_t backup;
	u_int16_t backup1, backup2;

	if(flow->http.url) {
	  ndpi_free(flow->http.url);
	  flow->http.url = NULL;
	}
	if(flow->http.content_type) {
	  ndpi_free(flow->http.content_type);
	  flow->http.content_type = NULL;
	}
	if(flow->http.user_agent) {
	  ndpi_free(flow->http.user_agent);
	  flow->http.user_agent = NULL;
	}
	if(flow->kerberos_buf.pktbuf) {
	  ndpi_free(flow->kerberos_buf.pktbuf);
	  flow->kerberos_buf.pktbuf = NULL;
	}
	if(flow->l4.tcp.tls.message.buffer) {
	  ndpi_free(flow->l4.tcp.tls.message.buffer);
	  flow->l4.tcp.tls.message.buffer = NULL;
	  flow->l4.tcp.tls.message.buffer_len = flow->l4.tcp.tls.message.buffer_used = 0;
	}

	backup = flow->num_processed_pkts;
	backup1 = flow->guessed_protocol_id;
	backup2 = flow->guessed_host_protocol_id;
	memset(flow, 0, sizeof(*(flow)));
	flow->num_processed_pkts = backup;
	flow->guessed_protocol_id = backup1;
	flow->guessed_host_protocol_id = backup2;

	NDPI_LOG_DBG(ndpi_str, "tcp syn packet for unknown protocol, reset detection state\n");
      }
    } else {
      /* tcp header not complete */
      flow->packet.tcp = NULL;
    }
  } else if(l4protocol == IPPROTO_UDP && flow->packet.l4_packet_len >= 8 /* size of udp */) {
    flow->packet.udp = (struct ndpi_udphdr *) l4ptr;
    flow->packet.payload_packet_len = flow->packet.l4_packet_len - 8;
    flow->packet.payload = ((u_int8_t *) flow->packet.udp) + 8;
  } else {
    flow->packet.generic_l4_ptr = l4ptr;
  }

  return(0);
}