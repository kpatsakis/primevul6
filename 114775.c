int ndpi_match_custom_category(struct ndpi_detection_module_struct *ndpi_str,
			       char *name, u_int name_len,
                               ndpi_protocol_category_t *category) {
  ndpi_protocol_breed_t breed;
  u_int16_t id;
  int rc = ndpi_match_string_protocol_id(ndpi_str->custom_categories.hostnames.ac_automa,
					 name, name_len, &id, category, &breed);

  return(rc);
}