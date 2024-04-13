void mg_http_serve_dir(struct mg_connection *c, struct mg_http_message *hm,
                       struct mg_http_serve_opts *opts) {
  char path[MG_PATH_MAX] = "";
  const char *sp = opts->ssi_pattern;
  int flags = uri_to_path(c, hm, opts, path, sizeof(path));
  if (flags == 0) return;
  LOG(LL_VERBOSE_DEBUG,
      ("%.*s %s %d", (int) hm->uri.len, hm->uri.ptr, path, flags));
  if (flags & MG_FS_DIR) {
    listdir(c, hm, opts, path);
  } else if (sp != NULL && mg_globmatch(sp, strlen(sp), path, strlen(path))) {
    mg_http_serve_ssi(c, opts->root_dir, path);
  } else {
    mg_http_serve_file(c, hm, path, opts);
  }
}