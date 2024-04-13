int ndpi_add_string_value_to_automa(void *_automa, char *str, u_int32_t num) {
  AC_PATTERN_t ac_pattern;
  AC_AUTOMATA_t *automa = (AC_AUTOMATA_t *) _automa;
  AC_ERROR_t rc;

  if(automa == NULL)
    return(-1);

  memset(&ac_pattern, 0, sizeof(ac_pattern));
  ac_pattern.astring    = str;
  ac_pattern.rep.number = num;
  ac_pattern.length     = strlen(ac_pattern.astring);

  rc = ac_automata_add(automa, &ac_pattern);
  return(rc == ACERR_SUCCESS || rc == ACERR_DUPLICATE_PATTERN ? 0 : -1);
}