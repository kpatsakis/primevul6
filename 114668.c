static int removeDefaultPort(ndpi_port_range *range, ndpi_proto_defaults_t *def, ndpi_default_ports_tree_node_t **root) {
  ndpi_default_ports_tree_node_t node;
  u_int16_t port;

  for (port = range->port_low; port <= range->port_high; port++) {
    ndpi_default_ports_tree_node_t *ret;

    node.proto = def, node.default_port = port;
    ret = (ndpi_default_ports_tree_node_t *) ndpi_tdelete(
							  &node, (void *) root, ndpi_default_ports_tree_node_t_cmp); /* Add it to the tree */

    if(ret != NULL) {
      ndpi_free((ndpi_default_ports_tree_node_t *) ret);
      return(0);
    }
  }

  return(-1);
}