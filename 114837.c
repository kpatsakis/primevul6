int ndpi_load_hostname_category(struct ndpi_detection_module_struct *ndpi_str, const char *name_to_add,
                                ndpi_protocol_category_t category) {
  char *name;

  if(name_to_add == NULL)
    return(-1);

  name = ndpi_strdup(name_to_add);

  if(name == NULL)
    return(-1);

#if 0
  printf("===> %s() Loading %s as %u\n", __FUNCTION__, name, category);
#endif

  AC_PATTERN_t ac_pattern;
  AC_ERROR_t rc;

  memset(&ac_pattern, 0, sizeof(ac_pattern));

  if(ndpi_str->custom_categories.hostnames_shadow.ac_automa == NULL) {
    free(name);
    return(-1);
  }

  ac_pattern.astring = name, ac_pattern.length = strlen(ac_pattern.astring);
  ac_pattern.rep.number = (u_int32_t) category,  ac_pattern.rep.category = category;;

  rc = ac_automata_add(ndpi_str->custom_categories.hostnames_shadow.ac_automa, &ac_pattern);
  if(rc != ACERR_DUPLICATE_PATTERN && rc != ACERR_SUCCESS) {
    free(name);
    return(-1);
  }

  if(rc == ACERR_DUPLICATE_PATTERN)
    free(name);

  return(0);
}