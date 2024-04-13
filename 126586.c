static size_t p_read(void *fp, void *buf, size_t len) {
  return fread(buf, 1, len, (FILE *) fp);
}