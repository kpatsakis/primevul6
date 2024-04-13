void ndpi_packet_dst_ip_get(const struct ndpi_packet_struct *packet, ndpi_ip_addr_t *ip) {
  NDPI_PROTOCOL_IP_clear(ip);

#ifdef NDPI_DETECTION_SUPPORT_IPV6

  if(packet->iphv6 != NULL) {
    ip->ipv6.u6_addr.u6_addr32[0] = packet->iphv6->ip6_dst.u6_addr.u6_addr32[0];
    ip->ipv6.u6_addr.u6_addr32[1] = packet->iphv6->ip6_dst.u6_addr.u6_addr32[1];
    ip->ipv6.u6_addr.u6_addr32[2] = packet->iphv6->ip6_dst.u6_addr.u6_addr32[2];
    ip->ipv6.u6_addr.u6_addr32[3] = packet->iphv6->ip6_dst.u6_addr.u6_addr32[3];

  } else

#endif

    ip->ipv4 = packet->iph->daddr;
}