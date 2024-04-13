void ndpi_init_protocol_match(struct ndpi_detection_module_struct *ndpi_str, ndpi_protocol_match *match) {
  u_int16_t no_master[2] = {NDPI_PROTOCOL_NO_MASTER_PROTO, NDPI_PROTOCOL_NO_MASTER_PROTO};
  ndpi_port_range ports_a[MAX_DEFAULT_PORTS], ports_b[MAX_DEFAULT_PORTS];

  if(ndpi_str->proto_defaults[match->protocol_id].protoName == NULL) {
    ndpi_str->proto_defaults[match->protocol_id].protoName = ndpi_strdup(match->proto_name);

    ndpi_str->proto_defaults[match->protocol_id].protoId = match->protocol_id;
    ndpi_str->proto_defaults[match->protocol_id].protoCategory = match->protocol_category;
    ndpi_str->proto_defaults[match->protocol_id].protoBreed = match->protocol_breed;

    ndpi_set_proto_defaults(ndpi_str, ndpi_str->proto_defaults[match->protocol_id].protoBreed,
			    ndpi_str->proto_defaults[match->protocol_id].protoId, 0 /* can_have_a_subprotocol */,
			    no_master, no_master, ndpi_str->proto_defaults[match->protocol_id].protoName,
			    ndpi_str->proto_defaults[match->protocol_id].protoCategory,
			    ndpi_build_default_ports(ports_a, 0, 0, 0, 0, 0) /* TCP */,
			    ndpi_build_default_ports(ports_b, 0, 0, 0, 0, 0) /* UDP */);
  }

  ndpi_add_host_url_subprotocol(ndpi_str, match->string_to_match, match->protocol_id, match->protocol_category,
				match->protocol_breed);
}