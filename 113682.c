static void lwp_write(FILE *fout, const std::string &buf) {
  size_t len = buf.length();
  fwrite(&len, sizeof(len), 1, fout);
  fwrite(buf.c_str(), sizeof(buf[0]), len, fout);
  fflush(fout);
}