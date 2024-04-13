static void static_cb(struct mg_connection *c, int ev, void *ev_data,
                      void *fn_data) {
  if (ev == MG_EV_WRITE || ev == MG_EV_POLL) {
    struct mg_fd *fd = (struct mg_fd *) fn_data;
    // Read to send IO buffer directly, avoid extra on-stack buffer
    size_t n, max = 2 * MG_IO_SIZE;
    if (c->send.size < max) mg_iobuf_resize(&c->send, max);
    if (c->send.len >= c->send.size) return;  // Rate limit
    n = fd->fs->read(fd->fd, c->send.buf + c->send.len,
                     c->send.size - c->send.len);
    if (n > 0) c->send.len += n;
    if (c->send.len < c->send.size) restore_http_cb(c);
  } else if (ev == MG_EV_CLOSE) {
    restore_http_cb(c);
  }
  (void) ev_data;
}