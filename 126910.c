static void reset_rrd_state(void) {
  optind = 0;
  opterr = 0;
  rrd_clear_error();
}