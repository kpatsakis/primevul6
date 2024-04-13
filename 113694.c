static void do_pclose(FILE *fin, FILE *fout) {
  int64_t fptr = 0;
  lwp_read_int64(fin, fptr);
  FILE *f = (FILE *)fptr;
  int ret = ::pclose(f);

  lwp_write_int32(fout, ret);
  if (ret < 0) {
    lwp_write_int32(fout, errno);
  }
  fflush(fout);
}