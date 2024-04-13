void ndpi_set_proto_category(struct ndpi_detection_module_struct *ndpi_str, u_int16_t protoId,
                             ndpi_protocol_category_t protoCategory) {
  if(protoId >= NDPI_MAX_SUPPORTED_PROTOCOLS + NDPI_MAX_NUM_CUSTOM_PROTOCOLS)
    return;
  else
    ndpi_str->proto_defaults[protoId].protoCategory = protoCategory;
}