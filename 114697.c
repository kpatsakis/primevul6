void NDPI_PROTOCOL_IP_clear(ndpi_ip_addr_t *ip) {
  memset(ip, 0, sizeof(ndpi_ip_addr_t));
}