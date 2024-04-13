int ndpi_load_ip_category(struct ndpi_detection_module_struct *ndpi_str, const char *ip_address_and_mask,
                          ndpi_protocol_category_t category) {
  patricia_node_t *node;
  struct in_addr pin;
  int bits = 32;
  char *ptr;
  char ipbuf[64];

  strncpy(ipbuf, ip_address_and_mask, sizeof(ipbuf));
  ipbuf[sizeof(ipbuf) - 1] = '\0';

  ptr = strrchr(ipbuf, '/');

  if(ptr) {
    *(ptr++) = '\0';
    if(atoi(ptr) >= 0 && atoi(ptr) <= 32)
      bits = atoi(ptr);
  }

  if(inet_pton(AF_INET, ipbuf, &pin) != 1) {
    NDPI_LOG_DBG2(ndpi_str, "Invalid ip/ip+netmask: %s\n", ip_address_and_mask);
    return(-1);
  }

  if((node = add_to_ptree(ndpi_str->custom_categories.ipAddresses_shadow, AF_INET, &pin, bits)) != NULL) {
    node->value.uv.user_value = (u_int16_t)category, node->value.uv.additional_user_value = 0;
  }

  return(0);
}