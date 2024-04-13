char *ndpi_get_proto_by_id(struct ndpi_detection_module_struct *ndpi_str, u_int id) {
  return((id >= ndpi_str->ndpi_num_supported_protocols) ? NULL : ndpi_str->proto_defaults[id].protoName);
}