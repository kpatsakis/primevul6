static void lwp_write_int32(FILE *fout, int32_t d) {
  fwrite(&d, sizeof(d), 1, fout);
  fflush(fout);
}