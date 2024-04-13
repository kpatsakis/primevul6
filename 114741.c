static int ndpi_string_to_automa(struct ndpi_detection_module_struct *ndpi_str, ndpi_automa *automa, char *value,
                                 u_int16_t protocol_id, ndpi_protocol_category_t category, ndpi_protocol_breed_t breed,
                                 u_int8_t free_str_on_duplicate) {
  AC_PATTERN_t ac_pattern;
  AC_ERROR_t rc;

  if((value == NULL) || (protocol_id >= (NDPI_MAX_SUPPORTED_PROTOCOLS + NDPI_MAX_NUM_CUSTOM_PROTOCOLS))) {
    NDPI_LOG_ERR(ndpi_str, "[NDPI] protoId=%d: INTERNAL ERROR\n", protocol_id);
    return(-1);
  }

  if(automa->ac_automa == NULL)
    return(-2);

  ac_pattern.astring = value, ac_pattern.rep.number = protocol_id,
    ac_pattern.rep.category = (u_int16_t) category, ac_pattern.rep.breed = (u_int16_t) breed;

#ifdef MATCH_DEBUG
  printf("Adding to automa [%s][protocol_id: %u][category: %u][breed: %u]\n", value, protocol_id, category, breed);
#endif

  if(value == NULL)
    ac_pattern.length = 0;
  else
    ac_pattern.length = strlen(ac_pattern.astring);

  rc = ac_automata_add(((AC_AUTOMATA_t *) automa->ac_automa), &ac_pattern);
  if(rc != ACERR_DUPLICATE_PATTERN && rc != ACERR_SUCCESS)
    return(-2);
  if(rc == ACERR_DUPLICATE_PATTERN && free_str_on_duplicate)
    ndpi_free(value);

  return(0);
}