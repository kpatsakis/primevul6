int ndpi_handle_rule(struct ndpi_detection_module_struct *ndpi_str, char *rule, u_int8_t do_add) {
  char *at, *proto, *elem;
  ndpi_proto_defaults_t *def;
  u_int16_t subprotocol_id, i;

  at = strrchr(rule, '@');
  if(at == NULL) {
    NDPI_LOG_ERR(ndpi_str, "Invalid rule '%s'\n", rule);
    return(-1);
  } else
    at[0] = 0, proto = &at[1];

  for (i = 0; proto[i] != '\0'; i++) {
    switch (proto[i]) {
    case '/':
    case '&':
    case '^':
    case ':':
    case ';':
    case '\'':
    case '"':
    case ' ':
      proto[i] = '_';
      break;
    }
  }

  for (i = 0, def = NULL; i < (int) ndpi_str->ndpi_num_supported_protocols; i++) {
    if(ndpi_str->proto_defaults[i].protoName && strcasecmp(ndpi_str->proto_defaults[i].protoName, proto) == 0) {
      def = &ndpi_str->proto_defaults[i];
      subprotocol_id = i;
      break;
    }
  }

  if(def == NULL) {
    if(!do_add) {
      /* We need to remove a rule */
      NDPI_LOG_ERR(ndpi_str, "Unable to find protocol '%s': skipping rule '%s'\n", proto, rule);
      return(-3);
    } else {
      ndpi_port_range ports_a[MAX_DEFAULT_PORTS], ports_b[MAX_DEFAULT_PORTS];
      u_int16_t no_master[2] = {NDPI_PROTOCOL_NO_MASTER_PROTO, NDPI_PROTOCOL_NO_MASTER_PROTO};

      if(ndpi_str->ndpi_num_custom_protocols >= (NDPI_MAX_NUM_CUSTOM_PROTOCOLS - 1)) {
	NDPI_LOG_ERR(ndpi_str, "Too many protocols defined (%u): skipping protocol %s\n",
		     ndpi_str->ndpi_num_custom_protocols, proto);
	return(-2);
      }

      ndpi_set_proto_defaults(
			      ndpi_str, NDPI_PROTOCOL_ACCEPTABLE, ndpi_str->ndpi_num_supported_protocols,
			      0 /* can_have_a_subprotocol */, no_master, no_master, proto,
			      NDPI_PROTOCOL_CATEGORY_UNSPECIFIED, /* TODO add protocol category support in rules */
			      ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			      ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
      def = &ndpi_str->proto_defaults[ndpi_str->ndpi_num_supported_protocols];
      subprotocol_id = ndpi_str->ndpi_num_supported_protocols;
      ndpi_str->ndpi_num_supported_protocols++, ndpi_str->ndpi_num_custom_protocols++;
    }
  }

  while ((elem = strsep(&rule, ",")) != NULL) {
    char *attr = elem, *value = NULL;
    ndpi_port_range range;
    int is_tcp = 0, is_udp = 0, is_ip = 0;

    if(strncmp(attr, "tcp:", 4) == 0)
      is_tcp = 1, value = &attr[4];
    else if(strncmp(attr, "udp:", 4) == 0)
      is_udp = 1, value = &attr[4];
    else if(strncmp(attr, "ip:", 3) == 0)
      is_ip = 1, value = &attr[3];
    else if(strncmp(attr, "host:", 5) == 0) {
      /* host:"<value>",host:"<value>",.....@<subproto> */
      value = &attr[5];
      if(value[0] == '"')
	value++; /* remove leading " */
      if(value[strlen(value) - 1] == '"')
	value[strlen(value) - 1] = '\0'; /* remove trailing " */
    }

    if(is_tcp || is_udp) {
      u_int p_low, p_high;

      if(sscanf(value, "%u-%u", &p_low, &p_high) == 2)
	range.port_low = p_low, range.port_high = p_high;
      else
	range.port_low = range.port_high = atoi(&elem[4]);

      if(do_add)
	addDefaultPort(ndpi_str, &range, def, 1 /* Custom user proto */,
		       is_tcp ? &ndpi_str->tcpRoot : &ndpi_str->udpRoot, __FUNCTION__, __LINE__);
      else
	removeDefaultPort(&range, def, is_tcp ? &ndpi_str->tcpRoot : &ndpi_str->udpRoot);
    } else if(is_ip) {
      /* NDPI_PROTOCOL_TOR */
      ndpi_add_host_ip_subprotocol(ndpi_str, value, subprotocol_id);
    } else {
      if(do_add)
	ndpi_add_host_url_subprotocol(ndpi_str, value, subprotocol_id, NDPI_PROTOCOL_CATEGORY_UNSPECIFIED,
				      NDPI_PROTOCOL_ACCEPTABLE);
      else
	ndpi_remove_host_url_subprotocol(ndpi_str, value, subprotocol_id);
    }
  }

  return(0);
}