static void lwp_read_int32(FILE *fin, int32_t &d) {
  fread(&d, sizeof(d), 1, fin);
}