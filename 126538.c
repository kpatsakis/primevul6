static int packed_stat(const char *path, size_t *size, time_t *mtime) {
  const char *p;
  size_t i, n = strlen(path);
  if (mg_unpack(path, size, mtime)) return MG_FS_READ;  // Regular file
  // Scan all files. If `path` is a dir prefix for any of them, it's a dir
  for (i = 0; (p = mg_unlist(i)) != NULL; i++) {
    if (is_dir_prefix(path, n, p)) return MG_FS_DIR;
  }
  return 0;
}