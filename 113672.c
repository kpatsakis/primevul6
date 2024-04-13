static void lwp_read_int64(FILE *fin, int64_t &d) {
  fread(&d, sizeof(d), 1, fin);
}