int ndpi_parse_ip_string(const char *ip_str, ndpi_ip_addr_t *parsed_ip) {
  int rv = -1;
  memset(parsed_ip, 0, sizeof(*parsed_ip));

  if(strchr(ip_str, '.')) {
    if(inet_pton(AF_INET, ip_str, &parsed_ip->ipv4) > 0)
      rv = 4;
#ifdef NDPI_DETECTION_SUPPORT_IPV6
  } else {
    if(inet_pton(AF_INET6, ip_str, &parsed_ip->ipv6) > 0)
      rv = 6;
#endif
  }

  return(rv);
}