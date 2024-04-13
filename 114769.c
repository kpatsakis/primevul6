int ndpi_enable_loaded_categories(struct ndpi_detection_module_struct *ndpi_str) {
  int i;

  /* First add the nDPI known categories matches */
  for (i = 0; category_match[i].string_to_match != NULL; i++)
    ndpi_load_category(ndpi_str, category_match[i].string_to_match, category_match[i].protocol_category);

  /* Free */
  ac_automata_release((AC_AUTOMATA_t *) ndpi_str->custom_categories.hostnames.ac_automa,
		      1 /* free patterns strings memory */);

  /* Finalize */
  ac_automata_finalize((AC_AUTOMATA_t *) ndpi_str->custom_categories.hostnames_shadow.ac_automa);

  /* Swap */
  ndpi_str->custom_categories.hostnames.ac_automa = ndpi_str->custom_categories.hostnames_shadow.ac_automa;

  /* Realloc */
  ndpi_str->custom_categories.hostnames_shadow.ac_automa = ac_automata_init(ac_match_handler);

  if(ndpi_str->custom_categories.ipAddresses != NULL)
    ndpi_Destroy_Patricia((patricia_tree_t *) ndpi_str->custom_categories.ipAddresses, free_ptree_data);

  ndpi_str->custom_categories.ipAddresses = ndpi_str->custom_categories.ipAddresses_shadow;
  ndpi_str->custom_categories.ipAddresses_shadow = ndpi_New_Patricia(32 /* IPv4 */);

  ndpi_str->custom_categories.categories_loaded = 1;

  return(0);
}