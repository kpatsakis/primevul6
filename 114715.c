void ndpi_free_automa(void *_automa) {
  ac_automata_release((AC_AUTOMATA_t *) _automa, 0);
}