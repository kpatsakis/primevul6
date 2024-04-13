static void packed_list(const char *dir, void (*fn)(const char *, void *),
                        void *userdata) {
  char buf[256], tmp[sizeof(buf)];
  const char *path, *begin, *end;
  size_t i, n = strlen(dir);
  tmp[0] = '\0';  // Previously listed entry
  for (i = 0; (path = mg_unlist(i)) != NULL; i++) {
    if (!is_dir_prefix(dir, n, path)) continue;
    begin = &path[n + 1];
    end = strchr(begin, '/');
    if (end == NULL) end = begin + strlen(begin);
    snprintf(buf, sizeof(buf), "%.*s", (int) (end - begin), begin);
    buf[sizeof(buf) - 1] = '\0';
    // If this entry has been already listed, skip
    // NOTE: we're assuming that file list is sorted alphabetically
    if (strcmp(buf, tmp) == 0) continue;
    fn(buf, userdata);  // Not yet listed, call user function
    strcpy(tmp, buf);   // And save this entry as listed
  }
}