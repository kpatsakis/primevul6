ndpi_protocol_category_t ndpi_get_proto_category(struct ndpi_detection_module_struct *ndpi_str,
						 ndpi_protocol proto) {
  if(proto.category != NDPI_PROTOCOL_CATEGORY_UNSPECIFIED)
    return(proto.category);

  /* simple rule: sub protocol first, master after */
  else if((proto.master_protocol == NDPI_PROTOCOL_UNKNOWN) ||
	  (ndpi_str->proto_defaults[proto.app_protocol].protoCategory != NDPI_PROTOCOL_CATEGORY_UNSPECIFIED)) {
    if(proto.app_protocol < (NDPI_MAX_SUPPORTED_PROTOCOLS + NDPI_MAX_NUM_CUSTOM_PROTOCOLS))
      return(ndpi_str->proto_defaults[proto.app_protocol].protoCategory);
  } else if(proto.master_protocol < (NDPI_MAX_SUPPORTED_PROTOCOLS + NDPI_MAX_NUM_CUSTOM_PROTOCOLS))
    return(ndpi_str->proto_defaults[proto.master_protocol].protoCategory);

  return(NDPI_PROTOCOL_CATEGORY_UNSPECIFIED);
}