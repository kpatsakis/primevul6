int ndpi_match_string_protocol_id(void *_automa, char *string_to_match,
				  u_int match_len, u_int16_t *protocol_id,
				  ndpi_protocol_category_t *category,
				  ndpi_protocol_breed_t *breed) {
  AC_TEXT_t ac_input_text;
  AC_AUTOMATA_t *automa = (AC_AUTOMATA_t *) _automa;
  AC_REP_t match = { 0, NDPI_PROTOCOL_CATEGORY_UNSPECIFIED, NDPI_PROTOCOL_UNRATED };
  int rc;

  *protocol_id = (u_int16_t)-1;
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
    *protocol_id = (u_int16_t)match.number, *category = match.category,
      *breed = match.breed;
  else
    *protocol_id = NDPI_PROTOCOL_UNKNOWN;

  return((*protocol_id != NDPI_PROTOCOL_UNKNOWN) ? 0 : -1);
}