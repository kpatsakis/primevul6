u_int8_t ndpi_is_proto(ndpi_protocol proto, u_int16_t p) {
  return(((proto.app_protocol == p) || (proto.master_protocol == p)) ? 1 : 0);
}