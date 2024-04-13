void mg_tls_handshake(struct mg_connection *c) {
  struct mg_tls *tls = (struct mg_tls *) c->tls;
  int rc;
  mbedtls_ssl_set_bio(&tls->ssl, &c->fd, mbedtls_net_send, mbedtls_net_recv, 0);
  rc = mbedtls_ssl_handshake(&tls->ssl);
  if (rc == 0) {  // Success
    LOG(LL_DEBUG, ("%lu success", c->id));
    c->is_tls_hs = 0;
  } else if (rc == MBEDTLS_ERR_SSL_WANT_READ ||
             rc == MBEDTLS_ERR_SSL_WANT_WRITE) {  // Still pending
    LOG(LL_VERBOSE_DEBUG, ("%lu pending, %d%d %d (-%#x)", c->id,
                           c->is_connecting, c->is_tls_hs, rc, -rc));
  } else {
    mg_error(c, "TLS handshake: -%#x", -rc);  // Error
  }
}