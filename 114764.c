static u_int8_t ndpi_detection_get_l4_internal(struct ndpi_detection_module_struct *ndpi_str, const u_int8_t *l3,
                                               u_int16_t l3_len, const u_int8_t **l4_return, u_int16_t *l4_len_return,
                                               u_int8_t *l4_protocol_return, u_int32_t flags) {
  const struct ndpi_iphdr *iph = NULL;
#ifdef NDPI_DETECTION_SUPPORT_IPV6
  const struct ndpi_ipv6hdr *iph_v6 = NULL;
#endif
  u_int16_t l4len = 0;
  const u_int8_t *l4ptr = NULL;
  u_int8_t l4protocol = 0;

  if(l3 == NULL || l3_len < sizeof(struct ndpi_iphdr))
    return(1);

  if((iph = (const struct ndpi_iphdr *) l3) == NULL)
    return(1);

  if(iph->version == IPVERSION && iph->ihl >= 5) {
    NDPI_LOG_DBG2(ndpi_str, "ipv4 header\n");
  }
#ifdef NDPI_DETECTION_SUPPORT_IPV6
  else if(iph->version == 6 && l3_len >= sizeof(struct ndpi_ipv6hdr)) {
    NDPI_LOG_DBG2(ndpi_str, "ipv6 header\n");
    iph_v6 = (const struct ndpi_ipv6hdr *) l3;
    iph = NULL;
  }
#endif
  else {
    return(1);
  }

  if((flags & NDPI_DETECTION_ONLY_IPV6) && iph != NULL) {
    NDPI_LOG_DBG2(ndpi_str, "ipv4 header found but excluded by flag\n");
    return(1);
  }
#ifdef NDPI_DETECTION_SUPPORT_IPV6
  else if((flags & NDPI_DETECTION_ONLY_IPV4) && iph_v6 != NULL) {
    NDPI_LOG_DBG2(ndpi_str, "ipv6 header found but excluded by flag\n");
    return(1);
  }
#endif

  if(iph != NULL && ndpi_iph_is_valid_and_not_fragmented(iph, l3_len)) {
    u_int16_t len = ntohs(iph->tot_len);
    u_int16_t hlen = (iph->ihl * 4);

    l4ptr = (((const u_int8_t *) iph) + iph->ihl * 4);

    if(len == 0)
      len = l3_len;

    l4len = (len > hlen) ? (len - hlen) : 0;
    l4protocol = iph->protocol;
  }
#ifdef NDPI_DETECTION_SUPPORT_IPV6
  else if(iph_v6 != NULL && (l3_len - sizeof(struct ndpi_ipv6hdr)) >= ntohs(iph_v6->ip6_hdr.ip6_un1_plen)) {
    l4ptr = (((const u_int8_t *) iph_v6) + sizeof(struct ndpi_ipv6hdr));
    l4len = ntohs(iph_v6->ip6_hdr.ip6_un1_plen);
    l4protocol = iph_v6->ip6_hdr.ip6_un1_nxt;

    // we need to handle IPv6 extension headers if present
    if(ndpi_handle_ipv6_extension_headers(ndpi_str, &l4ptr, &l4len, &l4protocol) != 0) {
      return(1);
    }

  }
#endif
  else {
    return(1);
  }

  if(l4_return != NULL) {
    *l4_return = l4ptr;
  }

  if(l4_len_return != NULL) {
    *l4_len_return = l4len;
  }

  if(l4_protocol_return != NULL) {
    *l4_protocol_return = l4protocol;
  }

  return(0);
}