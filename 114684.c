int ndpi_get_protocol_id_master_proto(struct ndpi_detection_module_struct *ndpi_str, u_int16_t protocol_id,
                                      u_int16_t **tcp_master_proto, u_int16_t **udp_master_proto) {
  if(protocol_id >= (NDPI_MAX_SUPPORTED_PROTOCOLS + NDPI_MAX_NUM_CUSTOM_PROTOCOLS)) {
    *tcp_master_proto = ndpi_str->proto_defaults[NDPI_PROTOCOL_UNKNOWN].master_tcp_protoId,
      *udp_master_proto = ndpi_str->proto_defaults[NDPI_PROTOCOL_UNKNOWN].master_udp_protoId;
    return(-1);
  }

  *tcp_master_proto = ndpi_str->proto_defaults[protocol_id].master_tcp_protoId,
    *udp_master_proto = ndpi_str->proto_defaults[protocol_id].master_udp_protoId;

  return(0);
}