u_int16_t ndpi_match_host_subprotocol(struct ndpi_detection_module_struct *ndpi_str, struct ndpi_flow_struct *flow,
                                      char *string_to_match, u_int string_to_match_len,
                                      ndpi_protocol_match_result *ret_match, u_int16_t master_protocol_id) {
  u_int16_t rc = ndpi_automa_match_string_subprotocol(ndpi_str, flow, string_to_match, string_to_match_len,
						      master_protocol_id, ret_match, 1);
  ndpi_protocol_category_t id = ret_match->protocol_category;

  if(ndpi_get_custom_category_match(ndpi_str, string_to_match, string_to_match_len, &id) != -1) {
    /* if(id != -1) */ {
      flow->category = ret_match->protocol_category = id;
      rc = master_protocol_id;
    }
  }

  return(rc);
}