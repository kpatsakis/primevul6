char *mg_file_read(const char *path, size_t *sizep) {
  FILE *fp;
  char *data = NULL;
  size_t size = 0;
  if ((fp = fopen(path, "rb")) != NULL) {
    fseek(fp, 0, SEEK_END);
    size = (size_t) ftell(fp);
    rewind(fp);
    data = (char *) calloc(1, size + 1);
    if (data != NULL) {
      if (fread(data, 1, size, fp) != size) {
        free(data);
        data = NULL;
      } else {
        data[size] = '\0';
        if (sizep != NULL) *sizep = size;
      }
    }
    fclose(fp);
  }
  return data;
}