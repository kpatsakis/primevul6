void ndpi_default_ports_tree_node_t_walker(const void *node, const ndpi_VISIT which, const int depth) {
  ndpi_default_ports_tree_node_t *f = *(ndpi_default_ports_tree_node_t **) node;

  printf("<%d>Walk on node %s (%u)\n", depth,
	 which == ndpi_preorder ?
	 "ndpi_preorder" :
	 which == ndpi_postorder ?
	 "ndpi_postorder" :
	 which == ndpi_endorder ? "ndpi_endorder" : which == ndpi_leaf ? "ndpi_leaf" : "unknown",
	 f->default_port);
}