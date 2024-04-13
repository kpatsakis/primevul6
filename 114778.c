u_int16_t ndpi_get_proto_by_name(struct ndpi_detection_module_struct *ndpi_str, const char *name) {
  u_int16_t i, num = ndpi_get_num_supported_protocols(ndpi_str);

  for (i = 0; i < num; i++)
    if(strcasecmp(ndpi_get_proto_by_id(ndpi_str, i), name) == 0)
      return(i);

  return(NDPI_PROTOCOL_UNKNOWN);
}