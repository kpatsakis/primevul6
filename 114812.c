int ndpi_ptree_match_addr(ndpi_ptree_t *tree,
			  const ndpi_ip_addr_t *addr, uint *user_data) {
  u_int8_t is_v6 = ndpi_is_ipv6(addr);
  patricia_tree_t *ptree = is_v6 ? tree->v6 : tree->v4;
  prefix_t prefix;
  patricia_node_t *node;
  int bits = ptree->maxbits;

  if(is_v6)
    fill_prefix_v6(&prefix, (const struct in6_addr *) &addr->ipv6, bits, ptree->maxbits);
  else
    fill_prefix_v4(&prefix, (const struct in_addr *) &addr->ipv4, bits, ptree->maxbits);

  node = ndpi_patricia_search_best(ptree, &prefix);

  if(node) {
    *user_data = node->value.uv.user_value;

    return(0);
  }

  return(-1);
}