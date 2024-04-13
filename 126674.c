void mg_tls_init(struct mg_connection *c, struct mg_tls_opts *opts) {
  struct mg_tls *tls = (struct mg_tls *) calloc(1, sizeof(*tls));
  const char *id = "mongoose";
  static unsigned char s_initialised = 0;
  int rc;

  if (tls == NULL) {
    mg_error(c, "TLS OOM");
    goto fail;
  }

  if (!s_initialised) {
    SSL_library_init();
    s_initialised++;
  }
  LOG(LL_DEBUG, ("%lu Setting TLS, CA: %s, cert: %s, key: %s", c->id,
                 opts->ca == NULL ? "null" : opts->ca,
                 opts->cert == NULL ? "null" : opts->cert,
                 opts->certkey == NULL ? "null" : opts->certkey));
  tls->ctx = c->is_client ? SSL_CTX_new(SSLv23_client_method())
                          : SSL_CTX_new(SSLv23_server_method());
  if ((tls->ssl = SSL_new(tls->ctx)) == NULL) {
    mg_error(c, "SSL_new");
    goto fail;
  }
  SSL_set_session_id_context(tls->ssl, (const uint8_t *) id,
                             (unsigned) strlen(id));
  // Disable deprecated protocols
  SSL_set_options(tls->ssl, SSL_OP_NO_SSLv2);
  SSL_set_options(tls->ssl, SSL_OP_NO_SSLv3);
  SSL_set_options(tls->ssl, SSL_OP_NO_TLSv1);
  SSL_set_options(tls->ssl, SSL_OP_NO_TLSv1_1);
#ifdef MG_ENABLE_OPENSSL_NO_COMPRESSION
  SSL_set_options(tls->ssl, SSL_OP_NO_COMPRESSION);
#endif
#ifdef MG_ENABLE_OPENSSL_CIPHER_SERVER_PREFERENCE
  SSL_set_options(tls->ssl, SSL_OP_CIPHER_SERVER_PREFERENCE);
#endif

  if (opts->ca != NULL && opts->ca[0] != '\0') {
    SSL_set_verify(tls->ssl, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
                   NULL);
    if ((rc = SSL_CTX_load_verify_locations(tls->ctx, opts->ca, NULL)) != 1) {
      mg_error(c, "parse(%s): err %d", opts->ca, mg_tls_err(tls, rc));
      goto fail;
    }
  }
  if (opts->cert != NULL && opts->cert[0] != '\0') {
    const char *key = opts->certkey;
    if (key == NULL) key = opts->cert;
    if ((rc = SSL_use_certificate_file(tls->ssl, opts->cert, 1)) != 1) {
      mg_error(c, "Invalid SSL cert, err %d", mg_tls_err(tls, rc));
      goto fail;
    } else if ((rc = SSL_use_PrivateKey_file(tls->ssl, key, 1)) != 1) {
      mg_error(c, "Invalid SSL key, err %d", mg_tls_err(tls, rc));
      goto fail;
#if OPENSSL_VERSION_NUMBER > 0x10100000L
    } else if ((rc = SSL_use_certificate_chain_file(tls->ssl, opts->cert)) !=
               1) {
      mg_error(c, "Invalid CA, err %d", mg_tls_err(tls, rc));
      goto fail;
#endif
    } else {
      SSL_set_mode(tls->ssl, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
#if OPENSSL_VERSION_NUMBER > 0x10002000L
      SSL_set_ecdh_auto(tls->ssl, 1);
#endif
    }
  }
  if (opts->ciphers != NULL) SSL_set_cipher_list(tls->ssl, opts->ciphers);
  if (opts->srvname.len > 0) {
    char mem[128], *buf = mem;
    mg_asprintf(&buf, sizeof(mem), "%.*s", (int) opts->srvname.len,
                opts->srvname.ptr);
    SSL_set_tlsext_host_name(tls->ssl, buf);
    if (buf != mem) free(buf);
  }
  c->tls = tls;
  c->is_tls = 1;
  c->is_tls_hs = 1;
  if (c->is_client && c->is_resolving == 0 && c->is_connecting == 0) {
    mg_tls_handshake(c);
  }
  LOG(LL_DEBUG, ("%lu SSL %s OK", c->id, c->is_accepted ? "accept" : "client"));
  return;
fail:
  c->is_closing = 1;
  free(tls);
}