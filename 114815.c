static ndpi_default_ports_tree_node_t *ndpi_get_guessed_protocol_id(struct ndpi_detection_module_struct *ndpi_str,
                                                                    u_int8_t proto, u_int16_t sport, u_int16_t dport) {
  ndpi_default_ports_tree_node_t node;

  if(sport && dport) {
    int low = ndpi_min(sport, dport);
    int high = ndpi_max(sport, dport);
    const void *ret;

    node.default_port = low; /* Check server port first */
    ret = ndpi_tfind(&node, (proto == IPPROTO_TCP) ? (void *) &ndpi_str->tcpRoot : (void *) &ndpi_str->udpRoot,
		     ndpi_default_ports_tree_node_t_cmp);

    if(ret == NULL) {
      node.default_port = high;
      ret = ndpi_tfind(&node, (proto == IPPROTO_TCP) ? (void *) &ndpi_str->tcpRoot : (void *) &ndpi_str->udpRoot,
		       ndpi_default_ports_tree_node_t_cmp);
    }

    if(ret)
      return(*(ndpi_default_ports_tree_node_t **) ret);
  }

  return(NULL);
}