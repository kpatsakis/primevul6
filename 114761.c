void ndpi_set_proto_breed(struct ndpi_detection_module_struct *ndpi_str, u_int16_t protoId, ndpi_protocol_breed_t breed) {
  if(protoId >= NDPI_MAX_SUPPORTED_PROTOCOLS + NDPI_MAX_NUM_CUSTOM_PROTOCOLS)
    return;
  else
    ndpi_str->proto_defaults[protoId].protoBreed = breed;
}