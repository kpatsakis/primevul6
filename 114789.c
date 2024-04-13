int ndpi_packet_dst_ip_eql(const struct ndpi_packet_struct *packet, const ndpi_ip_addr_t *ip) {
#ifdef NDPI_DETECTION_SUPPORT_IPV6

  /* IPv6 */
  if(packet->iphv6 != NULL) {
    if(packet->iphv6->ip6_dst.u6_addr.u6_addr32[0] == ip->ipv6.u6_addr.u6_addr32[0] &&
       packet->iphv6->ip6_dst.u6_addr.u6_addr32[1] == ip->ipv6.u6_addr.u6_addr32[1] &&
       packet->iphv6->ip6_dst.u6_addr.u6_addr32[2] == ip->ipv6.u6_addr.u6_addr32[2] &&
       packet->iphv6->ip6_dst.u6_addr.u6_addr32[3] == ip->ipv6.u6_addr.u6_addr32[3])
      return(1);
    //else
    return(0);
  }
#endif

  /* IPv4 */
  if(packet->iph->saddr == ip->ipv4)
    return(1);

  return(0);
}