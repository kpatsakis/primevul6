bool mg_file_write(const char *path, const void *buf, size_t len) {
  bool result = false;
  FILE *fp;
  char tmp[MG_PATH_MAX];
  snprintf(tmp, sizeof(tmp), "%s.%d", path, rand());
  fp = fopen(tmp, "wb");
  if (fp != NULL) {
    result = fwrite(buf, 1, len, fp) == len;
    fclose(fp);
    if (result) {
      remove(path);
      rename(tmp, path);
    } else {
      remove(tmp);
    }
  }
  return result;
}