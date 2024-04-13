int ndpi_fill_ip_protocol_category(struct ndpi_detection_module_struct *ndpi_str, u_int32_t saddr, u_int32_t daddr,
                                   ndpi_protocol *ret) {
  if(ndpi_str->custom_categories.categories_loaded) {
    prefix_t prefix;
    patricia_node_t *node;

    if(saddr == 0)
      node = NULL;
    else {
      /* Make sure all in network byte order otherwise compares wont work */
      fill_prefix_v4(&prefix, (struct in_addr *) &saddr, 32,
		     ((patricia_tree_t *) ndpi_str->protocols_ptree)->maxbits);
      node = ndpi_patricia_search_best(ndpi_str->custom_categories.ipAddresses, &prefix);
    }

    if(!node) {
      if(daddr != 0) {
	fill_prefix_v4(&prefix, (struct in_addr *) &daddr, 32,
		       ((patricia_tree_t *) ndpi_str->protocols_ptree)->maxbits);
	node = ndpi_patricia_search_best(ndpi_str->custom_categories.ipAddresses, &prefix);
      }
    }

    if(node) {
      ret->category = (ndpi_protocol_category_t) node->value.uv.user_value;

      return(1);
    }
  }

  ret->category = ndpi_get_proto_category(ndpi_str, *ret);

  return(0);
}