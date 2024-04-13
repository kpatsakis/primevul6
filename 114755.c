int ndpi_match_string_subprotocol(struct ndpi_detection_module_struct *ndpi_str, char *string_to_match,
                                  u_int string_to_match_len, ndpi_protocol_match_result *ret_match,
                                  u_int8_t is_host_match) {
  AC_TEXT_t ac_input_text;
  ndpi_automa *automa = is_host_match ? &ndpi_str->host_automa : &ndpi_str->content_automa;
  AC_REP_t match = {NDPI_PROTOCOL_UNKNOWN, NDPI_PROTOCOL_CATEGORY_UNSPECIFIED, NDPI_PROTOCOL_UNRATED};
  int rc;

  if((automa->ac_automa == NULL) || (string_to_match_len == 0))
    return(NDPI_PROTOCOL_UNKNOWN);

  if(!automa->ac_automa_finalized) {
    printf("[%s:%d] [NDPI] Internal error: please call ndpi_finalize_initalization()\n", __FILE__, __LINE__);
    return(0); /* No matches */
  }

  ac_input_text.astring = string_to_match, ac_input_text.length = string_to_match_len;
  rc = ac_automata_search(((AC_AUTOMATA_t *) automa->ac_automa), &ac_input_text, &match);

  /*
    As ac_automata_search can detect partial matches and continue the search process
    in case rc == 0 (i.e. no match), we need to check if there is a partial match
    and in this case return it
  */
  if((rc == 0) && (match.number != 0))
    rc = 1;

  /* We need to take into account also rc == 0 that is used for partial matches */
  ret_match->protocol_id = match.number, ret_match->protocol_category = match.category,
    ret_match->protocol_breed = match.breed;

  return(rc ? match.number : 0);
}