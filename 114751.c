static int ndpi_remove_host_url_subprotocol(struct ndpi_detection_module_struct *ndpi_str, char *value, int protocol_id) {
  NDPI_LOG_ERR(ndpi_str, "[NDPI] Missing implementation for proto %s/%d\n", value, protocol_id);
  return(-1);
}