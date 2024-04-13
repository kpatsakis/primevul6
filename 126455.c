static bool mg_is_chunked(struct mg_http_message *hm) {
  struct mg_str needle = mg_str_n("chunked", 7);
  struct mg_str *te = mg_http_get_header(hm, "Transfer-Encoding");
  return te != NULL && mg_strstr(*te, needle) != NULL;
}