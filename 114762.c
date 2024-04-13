u_int16_t ndpi_get_lower_proto(ndpi_protocol proto) {
  return((proto.master_protocol != NDPI_PROTOCOL_UNKNOWN) ? proto.master_protocol : proto.app_protocol);
}