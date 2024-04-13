int ndpi_ptree_insert(ndpi_ptree_t *tree, const ndpi_ip_addr_t *addr,
		      u_int8_t bits, uint user_data) {
  u_int8_t is_v6 = ndpi_is_ipv6(addr);
  patricia_tree_t *ptree = is_v6 ? tree->v6 : tree->v4;
  prefix_t prefix;
  patricia_node_t *node;

  if(bits > ptree->maxbits)
    return(-1);

  if(is_v6)
    fill_prefix_v6(&prefix, (const struct in6_addr *) &addr->ipv6, bits, ptree->maxbits);
  else
    fill_prefix_v4(&prefix, (const struct in_addr *) &addr->ipv4, bits, ptree->maxbits);

  /* Verify that the node does not already exist */
  node = ndpi_patricia_search_best(ptree, &prefix);

  if(node && (node->prefix->bitlen == bits))
    return(-2);

  node = ndpi_patricia_lookup(ptree, &prefix);

  if(node != NULL) {
    node->value.uv.user_value = user_data, node->value.uv.additional_user_value = 0;

    return(0);
  }

  return(-3);
}