char *ndpi_protocol2id(struct ndpi_detection_module_struct *ndpi_str,
		       ndpi_protocol proto, char *buf, u_int buf_len) {
  if((proto.master_protocol != NDPI_PROTOCOL_UNKNOWN) && (proto.master_protocol != proto.app_protocol)) {
    if(proto.app_protocol != NDPI_PROTOCOL_UNKNOWN)
      snprintf(buf, buf_len, "%u.%u", proto.master_protocol, proto.app_protocol);
    else
      snprintf(buf, buf_len, "%u", proto.master_protocol);
  } else
    snprintf(buf, buf_len, "%u", proto.app_protocol);

  return(buf);
}