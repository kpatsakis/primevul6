static int uri_to_path2(struct mg_connection *c, struct mg_http_message *hm,
                        struct mg_fs *fs, struct mg_str url, struct mg_str dir,
                        char *path, size_t path_size) {
  int flags = 0, tmp;
  // Append URI to the root_dir, and sanitize it
  size_t n = (size_t) snprintf(path, path_size, "%.*s", (int) dir.len, dir.ptr);
  if (n > path_size) n = path_size;
  path[path_size - 1] = '\0';
  if ((fs->stat(path, NULL, NULL) & MG_FS_DIR) == 0) {
    mg_http_reply(c, 400, "", "Invalid web root [%.*s]\n", (int) dir.len,
                  dir.ptr);
  } else {
    if (n + 2 < path_size) path[n++] = '/', path[n] = '\0';
    mg_url_decode(hm->uri.ptr + url.len, hm->uri.len - url.len, path + n,
                  path_size - n, 0);
    path[path_size - 1] = '\0';  // Double-check
    remove_double_dots(path);
    n = strlen(path);
    LOG(LL_VERBOSE_DEBUG,
        ("%lu %.*s -> %s", c->id, (int) hm->uri.len, hm->uri.ptr, path));
    while (n > 0 && path[n - 1] == '/') path[--n] = 0;  // Trim trailing slashes
    flags = fs->stat(path, NULL, NULL);                 // Does it exist?
    if (flags == 0) {
      mg_http_reply(c, 404, "", "Not found\n");  // Does not exist, doh
    } else if ((flags & MG_FS_DIR) && hm->uri.len > 0 &&
               hm->uri.ptr[hm->uri.len - 1] != '/') {
      mg_printf(c,
                "HTTP/1.1 301 Moved\r\n"
                "Location: %.*s/\r\n"
                "Content-Length: 0\r\n"
                "\r\n",
                (int) hm->uri.len, hm->uri.ptr);
      flags = 0;
    } else if (flags & MG_FS_DIR) {
      if (((snprintf(path + n, path_size - n, "/" MG_HTTP_INDEX) > 0 &&
            (tmp = fs->stat(path, NULL, NULL)) != 0) ||
           (snprintf(path + n, path_size - n, "/index.shtml") > 0 &&
            (tmp = fs->stat(path, NULL, NULL)) != 0))) {
        flags = tmp;
      } else {
        path[n] = '\0';  // Remove appended index file name
      }
    }
  }
  return flags;
}