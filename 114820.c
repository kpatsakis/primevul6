void *ndpi_init_automa(void) {
  return(ac_automata_init(ac_match_handler));
}