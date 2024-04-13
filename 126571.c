void mg_tls_handshake(struct mg_connection *c) {
  struct mg_tls *tls = (struct mg_tls *) c->tls;
  int rc;
  SSL_set_fd(tls->ssl, (int) (size_t) c->fd);
  rc = c->is_client ? SSL_connect(tls->ssl) : SSL_accept(tls->ssl);
  if (rc == 1) {
    LOG(LL_DEBUG, ("%lu success", c->id));
    c->is_tls_hs = 0;
  } else {
    int code = mg_tls_err(tls, rc);
    if (code != 0) mg_error(c, "tls hs: rc %d, err %d", rc, code);
  }
}