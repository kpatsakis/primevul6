const char *ndpi_get_l4_proto_name(ndpi_l4_proto_info proto) {
  switch (proto) {
  case ndpi_l4_proto_unknown:
    return("");
    break;

  case ndpi_l4_proto_tcp_only:
    return("TCP");
    break;

  case ndpi_l4_proto_udp_only:
    return("UDP");
    break;

  case ndpi_l4_proto_tcp_and_udp:
    return("TCP/UDP");
    break;
  }

  return("");
}