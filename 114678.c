void ndpi_set_proto_defaults(struct ndpi_detection_module_struct *ndpi_str, ndpi_protocol_breed_t breed,
                             u_int16_t protoId, u_int8_t can_have_a_subprotocol, u_int16_t tcp_master_protoId[2],
                             u_int16_t udp_master_protoId[2], char *protoName, ndpi_protocol_category_t protoCategory,
                             ndpi_port_range *tcpDefPorts, ndpi_port_range *udpDefPorts) {
  char *name;
  int j;

  if(protoId >= NDPI_MAX_SUPPORTED_PROTOCOLS + NDPI_MAX_NUM_CUSTOM_PROTOCOLS) {
#ifdef DEBUG
    NDPI_LOG_ERR(ndpi_str, "[NDPI] %s/protoId=%d: INTERNAL ERROR\n", protoName, protoId);
#endif
    return;
  }

  if(ndpi_str->proto_defaults[protoId].protoName != NULL) {
#ifdef DEBUG
    NDPI_LOG_ERR(ndpi_str, "[NDPI] %s/protoId=%d: already initialized. Ignoring it\n", protoName, protoId);
#endif
    return;
  }

  name = ndpi_strdup(protoName);

  if(ndpi_str->proto_defaults[protoId].protoName)
    ndpi_free(ndpi_str->proto_defaults[protoId].protoName);

  ndpi_str->proto_defaults[protoId].protoName = name, ndpi_str->proto_defaults[protoId].protoCategory = protoCategory,
    ndpi_str->proto_defaults[protoId].protoId = protoId, ndpi_str->proto_defaults[protoId].protoBreed = breed;
  ndpi_str->proto_defaults[protoId].can_have_a_subprotocol = can_have_a_subprotocol;

  memcpy(&ndpi_str->proto_defaults[protoId].master_tcp_protoId, tcp_master_protoId, 2 * sizeof(u_int16_t));
  memcpy(&ndpi_str->proto_defaults[protoId].master_udp_protoId, udp_master_protoId, 2 * sizeof(u_int16_t));

  for (j = 0; j < MAX_DEFAULT_PORTS; j++) {
    if(udpDefPorts[j].port_low != 0)
      addDefaultPort(ndpi_str, &udpDefPorts[j], &ndpi_str->proto_defaults[protoId], 0, &ndpi_str->udpRoot,
		     __FUNCTION__, __LINE__);

    if(tcpDefPorts[j].port_low != 0)
      addDefaultPort(ndpi_str, &tcpDefPorts[j], &ndpi_str->proto_defaults[protoId], 0, &ndpi_str->tcpRoot,
		     __FUNCTION__, __LINE__);

    /* No port range, just the lower port */
    ndpi_str->proto_defaults[protoId].tcp_default_ports[j] = tcpDefPorts[j].port_low;
    ndpi_str->proto_defaults[protoId].udp_default_ports[j] = udpDefPorts[j].port_low;
  }
}