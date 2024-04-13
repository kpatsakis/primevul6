static int ndpi_add_host_ip_subprotocol(struct ndpi_detection_module_struct *ndpi_str,
					char *value, u_int16_t protocol_id) {
  patricia_node_t *node;
  struct in_addr pin;
  int bits = 32;
  char *ptr = strrchr(value, '/');
  u_int16_t port = 0; /* Format ip:8.248.73.247:443 */
  char *double_column;

  if(ptr) {
    ptr[0] = '\0';
    ptr++;

    if((double_column = strrchr(ptr, ':')) != NULL) {
      double_column[0] = '\0';
      port = atoi(&double_column[1]);
    }

    if(atoi(ptr) >= 0 && atoi(ptr) <= 32)
      bits = atoi(ptr);
  } else {
    /*
      Let's check if there is the port defined
      Example: ip:8.248.73.247:443@AmazonPrime
    */
    double_column = strrchr(value, ':');

    if(double_column) {
      double_column[0] = '\0';
      port = atoi(&double_column[1]);
    }
  }

  inet_pton(AF_INET, value, &pin);

  if((node = add_to_ptree(ndpi_str->protocols_ptree, AF_INET, &pin, bits)) != NULL) {
    node->value.uv.user_value = protocol_id, node->value.uv.additional_user_value = htons(port);
  }

  return(0);
}