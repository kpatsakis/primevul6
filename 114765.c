u_int16_t ndpi_network_port_ptree_match(struct ndpi_detection_module_struct *ndpi_str,
					struct in_addr *pin /* network byte order */,
					u_int16_t port /* network byte order */) {
  prefix_t prefix;
  patricia_node_t *node;

  /* Make sure all in network byte order otherwise compares wont work */
  fill_prefix_v4(&prefix, pin, 32, ((patricia_tree_t *) ndpi_str->protocols_ptree)->maxbits);
  node = ndpi_patricia_search_best(ndpi_str->protocols_ptree, &prefix);

  if(node) {
    if((node->value.uv.additional_user_value == 0)
       || (node->value.uv.additional_user_value == port))
      return(node->value.uv.user_value);
  }

  return(NDPI_PROTOCOL_UNKNOWN);
}