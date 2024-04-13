static void init_string_based_protocols(struct ndpi_detection_module_struct *ndpi_str) {
  int i;

  for (i = 0; host_match[i].string_to_match != NULL; i++)
    ndpi_init_protocol_match(ndpi_str, &host_match[i]);

  ndpi_enable_loaded_categories(ndpi_str);

#ifdef MATCH_DEBUG
  // ac_automata_display(ndpi_str->host_automa.ac_automa, 'n');
#endif

  for (i = 0; ndpi_en_bigrams[i] != NULL; i++)
    ndpi_string_to_automa(ndpi_str, &ndpi_str->bigrams_automa, (char *) ndpi_en_bigrams[i], 1, 1, 1, 0);

  for (i = 0; ndpi_en_impossible_bigrams[i] != NULL; i++)
    ndpi_string_to_automa(ndpi_str, &ndpi_str->impossible_bigrams_automa, (char *) ndpi_en_impossible_bigrams[i], 1,
			  1, 1, 0);
}