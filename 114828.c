void ndpi_exit_detection_module(struct ndpi_detection_module_struct *ndpi_str) {
  if(ndpi_str != NULL) {
    int i;

    for (i = 0; i < (NDPI_MAX_SUPPORTED_PROTOCOLS + NDPI_MAX_NUM_CUSTOM_PROTOCOLS); i++) {
      if(ndpi_str->proto_defaults[i].protoName)
	ndpi_free(ndpi_str->proto_defaults[i].protoName);
    }

    /* NDPI_PROTOCOL_TINC */
    if(ndpi_str->tinc_cache)
      cache_free((cache_t)(ndpi_str->tinc_cache));

    if(ndpi_str->ookla_cache)
      ndpi_lru_free_cache(ndpi_str->ookla_cache);

    if(ndpi_str->stun_cache)
      ndpi_lru_free_cache(ndpi_str->stun_cache);

    if(ndpi_str->msteams_cache)
      ndpi_lru_free_cache(ndpi_str->msteams_cache);

    if(ndpi_str->protocols_ptree)
      ndpi_Destroy_Patricia((patricia_tree_t *) ndpi_str->protocols_ptree, free_ptree_data);

    if(ndpi_str->udpRoot != NULL)
      ndpi_tdestroy(ndpi_str->udpRoot, ndpi_free);
    if(ndpi_str->tcpRoot != NULL)
      ndpi_tdestroy(ndpi_str->tcpRoot, ndpi_free);

    if(ndpi_str->host_automa.ac_automa != NULL)
      ac_automata_release((AC_AUTOMATA_t *) ndpi_str->host_automa.ac_automa,
			  1 /* free patterns strings memory */);

    if(ndpi_str->content_automa.ac_automa != NULL)
      ac_automata_release((AC_AUTOMATA_t *) ndpi_str->content_automa.ac_automa, 0);

    if(ndpi_str->bigrams_automa.ac_automa != NULL)
      ac_automata_release((AC_AUTOMATA_t *) ndpi_str->bigrams_automa.ac_automa, 0);

    if(ndpi_str->impossible_bigrams_automa.ac_automa != NULL)
      ac_automata_release((AC_AUTOMATA_t *) ndpi_str->impossible_bigrams_automa.ac_automa, 0);

    if(ndpi_str->custom_categories.hostnames.ac_automa != NULL)
      ac_automata_release((AC_AUTOMATA_t *) ndpi_str->custom_categories.hostnames.ac_automa,
			  1 /* free patterns strings memory */);

    if(ndpi_str->custom_categories.hostnames_shadow.ac_automa != NULL)
      ac_automata_release((AC_AUTOMATA_t *) ndpi_str->custom_categories.hostnames_shadow.ac_automa,
			  1 /* free patterns strings memory */);

    if(ndpi_str->custom_categories.ipAddresses != NULL)
      ndpi_Destroy_Patricia((patricia_tree_t *) ndpi_str->custom_categories.ipAddresses, free_ptree_data);

    if(ndpi_str->custom_categories.ipAddresses_shadow != NULL)
      ndpi_Destroy_Patricia((patricia_tree_t *) ndpi_str->custom_categories.ipAddresses_shadow, free_ptree_data);

#ifdef CUSTOM_NDPI_PROTOCOLS
#include "../../../nDPI-custom/ndpi_exit_detection_module.c"
#endif

    ndpi_free(ndpi_str);
  }
}