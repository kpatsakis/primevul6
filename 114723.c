void ndpi_finalize_initalization(struct ndpi_detection_module_struct *ndpi_str) {
  u_int i;

  for (i = 0; i < 4; i++) {
    ndpi_automa *automa;

    switch (i) {
    case 0:
      automa = &ndpi_str->host_automa;
      break;

    case 1:
      automa = &ndpi_str->content_automa;
      break;

    case 2:
      automa = &ndpi_str->bigrams_automa;
      break;

    case 3:
      automa = &ndpi_str->impossible_bigrams_automa;
      break;

    default:
      automa = NULL;
      break;
    }

    if(automa) {
      ac_automata_finalize((AC_AUTOMATA_t *) automa->ac_automa);
      automa->ac_automa_finalized = 1;
    }
  }
}