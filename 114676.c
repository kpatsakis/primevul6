static void addDefaultPort(struct ndpi_detection_module_struct *ndpi_str, ndpi_port_range *range,
                           ndpi_proto_defaults_t *def, u_int8_t customUserProto, ndpi_default_ports_tree_node_t **root,
                           const char *_func, int _line) {
  u_int16_t port;

  for (port = range->port_low; port <= range->port_high; port++) {
    ndpi_default_ports_tree_node_t *node =
      (ndpi_default_ports_tree_node_t *) ndpi_malloc(sizeof(ndpi_default_ports_tree_node_t));
    ndpi_default_ports_tree_node_t *ret;

    if(!node) {
      NDPI_LOG_ERR(ndpi_str, "%s:%d not enough memory\n", _func, _line);
      break;
    }

    node->proto = def, node->default_port = port, node->customUserProto = customUserProto;
    ret = (ndpi_default_ports_tree_node_t *) ndpi_tsearch(node, (void *) root, ndpi_default_ports_tree_node_t_cmp); /* Add it to the tree */

    if(ret != node) {
      NDPI_LOG_DBG(ndpi_str, "[NDPI] %s:%d found duplicate for port %u: overwriting it with new value\n", _func,
		   _line, port);

      ret->proto = def;
      ndpi_free(node);
    }
  }
}