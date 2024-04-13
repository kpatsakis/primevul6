int NDPI_PROTOCOL_IP_is_set(const ndpi_ip_addr_t *ip) {
  return(memcmp(ip, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", sizeof(ndpi_ip_addr_t)) != 0);
}