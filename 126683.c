void mg_http_serve_ssi(struct mg_connection *c, const char *root,
                       const char *fullpath) {
  const char *headers = "Content-Type: text/html; charset=utf-8\r\n";
  char *data = mg_ssi(fullpath, root, 0);
  mg_http_reply(c, 200, headers, "%s", data == NULL ? "" : data);
  free(data);
}