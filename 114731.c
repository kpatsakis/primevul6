int ndpi_match_string_value(void *_automa, char *string_to_match,
			    u_int match_len, u_int32_t *num) {
  AC_TEXT_t ac_input_text;
  AC_AUTOMATA_t *automa = (AC_AUTOMATA_t *) _automa;
  AC_REP_t match = { 0, NDPI_PROTOCOL_CATEGORY_UNSPECIFIED, NDPI_PROTOCOL_UNRATED };
  int rc;

  *num = (u_int32_t)-1;
  if((automa == NULL) || (string_to_match == NULL) || (string_to_match[0] == '\0'))
    return(-2);

  ac_input_text.astring = string_to_match, ac_input_text.length = match_len;
  rc = ac_automata_search(automa, &ac_input_text, &match);

  /*
    As ac_automata_search can detect partial matches and continue the search process
    in case rc == 0 (i.e. no match), we need to check if there is a partial match
    and in this case return it
  */
  if((rc == 0) && (match.number != 0))
    rc = 1;

  if(rc)
    *num = match.number;
  else
    *num = 0;

  return(rc ? 0 : -1);
}