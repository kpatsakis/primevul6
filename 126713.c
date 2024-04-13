static long read_conn(struct mg_connection *c) {
  long n = -1;
  if (c->recv.len >= MG_MAX_RECV_BUF_SIZE) {
    mg_error(c, "max_recv_buf_size reached");
  } else if (c->recv.size - c->recv.len < MG_IO_SIZE &&
             !mg_iobuf_resize(&c->recv, c->recv.size + MG_IO_SIZE)) {
    mg_error(c, "oom");
  } else {
    char *buf = (char *) &c->recv.buf[c->recv.len];
    size_t len = c->recv.size - c->recv.len;
    n = c->is_tls ? mg_tls_recv(c, buf, len) : mg_sock_recv(c, buf, len);
    iolog(c, buf, n, true);
  }
  return n;
}