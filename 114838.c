int ndpi_match_bigram(struct ndpi_detection_module_struct *ndpi_str,
		      ndpi_automa *automa, char *bigram_to_match) {
  AC_TEXT_t ac_input_text;
  AC_REP_t match = {NDPI_PROTOCOL_UNKNOWN, NDPI_PROTOCOL_CATEGORY_UNSPECIFIED, NDPI_PROTOCOL_UNRATED};
  int rc;

  if((automa->ac_automa == NULL) || (bigram_to_match == NULL))
    return(-1);

  if(!automa->ac_automa_finalized) {
    printf("[%s:%d] [NDPI] Internal error: please call ndpi_finalize_initalization()\n", __FILE__, __LINE__);
    return(0); /* No matches */
  }

  ac_input_text.astring = bigram_to_match, ac_input_text.length = 2;
  rc = ac_automata_search(((AC_AUTOMATA_t *) automa->ac_automa), &ac_input_text, &match);

  /*
    As ac_automata_search can detect partial matches and continue the search process
    in case rc == 0 (i.e. no match), we need to check if there is a partial match
    and in this case return it
  */
  if((rc == 0) && (match.number != 0))
    rc = 1;

  return(rc ? match.number : 0);
}