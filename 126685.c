bool mg_send(struct mg_connection *c, const void *buf, size_t len) {
  if (c->is_udp) {
    long n = mg_sock_send(c, buf, len);
    iolog(c, (char *) buf, n, false);
    return n > 0;
  } else {
    return mg_iobuf_add(&c->send, c->send.len, buf, len, MG_IO_SIZE);
  }
}