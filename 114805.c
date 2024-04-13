int ndpi_match_string(void *_automa, char *string_to_match) {
  AC_REP_t match = { NDPI_PROTOCOL_UNKNOWN, NDPI_PROTOCOL_CATEGORY_UNSPECIFIED, NDPI_PROTOCOL_UNRATED };
  AC_TEXT_t ac_input_text;
  AC_AUTOMATA_t *automa = (AC_AUTOMATA_t *) _automa;
  int rc;

  if((automa == NULL) || (string_to_match == NULL) || (string_to_match[0] == '\0'))
    return(-2);

  ac_input_text.astring = string_to_match, ac_input_text.length = strlen(string_to_match);
  rc = ac_automata_search(automa, &ac_input_text, &match);

  /*
    As ac_automata_search can detect partial matches and continue the search process
    in case rc == 0 (i.e. no match), we need to check if there is a partial match
    and in this case return it
  */
  if((rc == 0) && (match.number != 0))
    rc = 1;

  return(rc ? match.number : 0);
}