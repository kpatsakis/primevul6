int ndpi_get_custom_category_match(struct ndpi_detection_module_struct *ndpi_str,
				   char *name_or_ip, u_int name_len,
				   ndpi_protocol_category_t *id) {
  char ipbuf[64], *ptr;
  struct in_addr pin;
  u_int cp_len = ndpi_min(sizeof(ipbuf) - 1, name_len);

  if(!ndpi_str->custom_categories.categories_loaded)
    return(-1);

  if(cp_len > 0) {
    memcpy(ipbuf, name_or_ip, cp_len);
    ipbuf[cp_len] = '\0';
  } else
    ipbuf[0] = '\0';

  ptr = strrchr(ipbuf, '/');

  if(ptr)
    ptr[0] = '\0';

  if(inet_pton(AF_INET, ipbuf, &pin) == 1) {
    /* Search IP */
    prefix_t prefix;
    patricia_node_t *node;

    /* Make sure all in network byte order otherwise compares wont work */
    fill_prefix_v4(&prefix, &pin, 32, ((patricia_tree_t *) ndpi_str->protocols_ptree)->maxbits);
    node = ndpi_patricia_search_best(ndpi_str->custom_categories.ipAddresses, &prefix);

    if(node) {
      *id = node->value.uv.user_value;

      return(0);
    }

    return(-1);
  } else {
    /* Search Host */
    return(ndpi_match_custom_category(ndpi_str, name_or_ip, name_len, id));
  }
}