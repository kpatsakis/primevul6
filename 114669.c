int ndpi_get_protocol_id(struct ndpi_detection_module_struct *ndpi_str, char *proto) {
  int i;

  for (i = 0; i < (int) ndpi_str->ndpi_num_supported_protocols; i++)
    if(strcasecmp(proto, ndpi_str->proto_defaults[i].protoName) == 0)
      return(i);

  return(-1);
}