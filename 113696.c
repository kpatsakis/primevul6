static void lwp_read(FILE *fin, std::string &buf) {
  size_t len;
  fread(&len, sizeof(len), 1, fin);
  char *buffer = (char *)malloc(len + 1);
  fread(buffer, sizeof(*buffer), len, fin);
  buffer[len] = '\0';
  buf = std::string(buffer);
  free(buffer);
}