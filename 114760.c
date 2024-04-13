static u_int8_t ndpi_iph_is_valid_and_not_fragmented(const struct ndpi_iphdr *iph, const u_int16_t ipsize) {
  //#ifdef REQUIRE_FULL_PACKETS
  if(ipsize < iph->ihl * 4 || ipsize < ntohs(iph->tot_len) || ntohs(iph->tot_len) < iph->ihl * 4 ||
     (iph->frag_off & htons(0x1FFF)) != 0) {
    return(0);
  }
  //#endif

  return(1);
}