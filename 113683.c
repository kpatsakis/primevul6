static void lwp_write_int64(FILE *fout, int64_t d) {
  fwrite(&d, sizeof(d), 1, fout);
  fflush(fout);
}