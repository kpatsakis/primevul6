static size_t p_write(void *fp, const void *buf, size_t len) {
  return fwrite(buf, 1, len, (FILE *) fp);
}