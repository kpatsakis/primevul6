char *ndpi_get_ip_string(const ndpi_ip_addr_t *ip, char *buf, u_int buf_len) {
  const u_int8_t *a = (const u_int8_t *) &ip->ipv4;

#ifdef NDPI_DETECTION_SUPPORT_IPV6
  if(ndpi_is_ipv6(ip)) {
    if(inet_ntop(AF_INET6, &ip->ipv6.u6_addr, buf, buf_len) == NULL)
      buf[0] = '\0';

    return(buf);
  }
#endif

  snprintf(buf, buf_len, "%u.%u.%u.%u", a[0], a[1], a[2], a[3]);

  return(buf);
}