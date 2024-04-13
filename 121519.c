static int search_ssh_again(struct ndpi_detection_module_struct *ndpi_struct, struct ndpi_flow_struct *flow) {
  ndpi_search_ssh_tcp(ndpi_struct, flow);

  if((flow->protos.ssh.hassh_client[0] != '\0')
     && (flow->protos.ssh.hassh_server[0] != '\0')) {
    /* stop extra processing */
    flow->extra_packets_func = NULL; /* We're good now */
    return(0);
  }

  /* Possibly more processing */
  return(1);
}