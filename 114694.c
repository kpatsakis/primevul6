static patricia_node_t *add_to_ptree(patricia_tree_t *tree, int family, void *addr, int bits) {
  prefix_t prefix;
  patricia_node_t *node;

  fill_prefix_v4(&prefix, (struct in_addr *) addr, bits, tree->maxbits);

  node = ndpi_patricia_lookup(tree, &prefix);
  if(node) memset(&node->value, 0, sizeof(node->value));

  return(node);
}