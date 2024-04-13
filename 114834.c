static void ndpi_init_ptree_ipv4(struct ndpi_detection_module_struct *ndpi_str,
				 void *ptree, ndpi_network host_list[],
                                 u_int8_t skip_tor_hosts) {
  int i;

  for (i = 0; host_list[i].network != 0x0; i++) {
    struct in_addr pin;
    patricia_node_t *node;

    if(skip_tor_hosts && (host_list[i].value == NDPI_PROTOCOL_TOR))
      continue;

    pin.s_addr = htonl(host_list[i].network);
    if((node = add_to_ptree(ptree, AF_INET, &pin, host_list[i].cidr /* bits */)) != NULL) {
      node->value.uv.user_value = host_list[i].value, node->value.uv.additional_user_value = 0;
    }
  }
}