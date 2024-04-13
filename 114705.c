u_int8_t ndpi_is_subprotocol_informative(struct ndpi_detection_module_struct *ndpi_str, u_int16_t protoId) {
  if(protoId >= NDPI_MAX_SUPPORTED_PROTOCOLS + NDPI_MAX_NUM_CUSTOM_PROTOCOLS)
    return(0);

  switch (protoId) {
    /* All dissectors that have calls to ndpi_match_host_subprotocol() */
  case NDPI_PROTOCOL_DNS:
    return(1);
    break;

  default:
    return(0);
  }
}