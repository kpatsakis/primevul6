u_int8_t ndpi_is_ipv6(const ndpi_ip_addr_t *ip) {
#ifdef NDPI_DETECTION_SUPPORT_IPV6
  return(ip->ipv6.u6_addr.u6_addr32[1] != 0 || ip->ipv6.u6_addr.u6_addr32[2] != 0 ||
	  ip->ipv6.u6_addr.u6_addr32[3] != 0);
#else
  return(0);
#endif
}