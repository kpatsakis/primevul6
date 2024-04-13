ndpi_protocol_breed_t ndpi_get_proto_breed(struct ndpi_detection_module_struct *ndpi_str,
					   u_int16_t proto_id) {
  if((proto_id >= ndpi_str->ndpi_num_supported_protocols) ||
     (proto_id >= (NDPI_MAX_SUPPORTED_PROTOCOLS + NDPI_MAX_NUM_CUSTOM_PROTOCOLS)) ||
     (ndpi_str->proto_defaults[proto_id].protoName == NULL))
    proto_id = NDPI_PROTOCOL_UNKNOWN;

  return(ndpi_str->proto_defaults[proto_id].protoBreed);
}