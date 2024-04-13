size_t mg_ws_send(struct mg_connection *c, const char *buf, size_t len,
                  int op) {
  uint8_t header[14];
  size_t header_len = mkhdr(len, op, c->is_client, header);
  mg_send(c, header, header_len);
  LOG(LL_VERBOSE_DEBUG, ("WS out: %d [%.*s]", (int) len, (int) len, buf));
  mg_send(c, buf, len);
  mg_ws_mask(c, len);
  return header_len + len;
}