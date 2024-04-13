static u_int8_t tor_ptree_match(struct ndpi_detection_module_struct *ndpi_str, struct in_addr *pin) {
  return((ndpi_network_ptree_match(ndpi_str, pin) == NDPI_PROTOCOL_TOR) ? 1 : 0);
}