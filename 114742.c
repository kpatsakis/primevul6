int ndpi_match_hostname_protocol(struct ndpi_detection_module_struct *ndpi_struct, struct ndpi_flow_struct *flow,
                                 u_int16_t master_protocol, char *name, u_int name_len) {
  ndpi_protocol_match_result ret_match;
  u_int16_t subproto, what_len;
  char *what;

  if((name_len > 2) && (name[0] == '*') && (name[1] == '.'))
    what = &name[1], what_len = name_len - 1;
  else
    what = name, what_len = name_len;

  subproto = ndpi_match_host_subprotocol(ndpi_struct, flow, what, what_len, &ret_match, master_protocol);

  if(subproto != NDPI_PROTOCOL_UNKNOWN) {
    ndpi_set_detected_protocol(ndpi_struct, flow, subproto, master_protocol);
    ndpi_int_change_category(ndpi_struct, flow, ret_match.protocol_category);
    return(1);
  } else
    return(0);
}