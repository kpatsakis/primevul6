static int is_dir_prefix(const char *prefix, size_t n, const char *path) {
  return n < strlen(path) && memcmp(prefix, path, n) == 0 && path[n] == '/';
  //(n == 0 || path[n] == MG_DIRSEP);
}