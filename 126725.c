void mg_tls_init(struct mg_connection *c, struct mg_tls_opts *opts) {
  struct mg_tls *tls = (struct mg_tls *) calloc(1, sizeof(*tls));
  int rc = 0;
  const char *ca = opts->ca == NULL     ? "-"
                   : opts->ca[0] == '-' ? "(emb)"
                                        : opts->ca;
  const char *crl = opts->crl == NULL     ? "-"
                    : opts->crl[0] == '-' ? "(emb)"
                                          : opts->crl;
  const char *cert = opts->cert == NULL     ? "-"
                     : opts->cert[0] == '-' ? "(emb)"
                                            : opts->cert;
  const char *certkey = opts->certkey == NULL     ? "-"
                        : opts->certkey[0] == '-' ? "(emb)"
                                                  : opts->certkey;
  if (tls == NULL) {
    mg_error(c, "TLS OOM");
    goto fail;
  }
  LOG(LL_DEBUG, ("%lu Setting TLS, CA: %s, CRL: %s, cert: %s, key: %s", c->id,
                 ca, crl, cert, certkey));
  mbedtls_ssl_init(&tls->ssl);
  mbedtls_ssl_config_init(&tls->conf);
  mbedtls_x509_crt_init(&tls->ca);
  mbedtls_x509_crl_init(&tls->crl);
  mbedtls_x509_crt_init(&tls->cert);
  mbedtls_pk_init(&tls->pk);
  mbedtls_ssl_conf_dbg(&tls->conf, debug_cb, c);
  //#if !defined(ESP_PLATFORM)
  // mbedtls_debug_set_threshold(5);
  //#endif
  if ((rc = mbedtls_ssl_config_defaults(
           &tls->conf,
           c->is_client ? MBEDTLS_SSL_IS_CLIENT : MBEDTLS_SSL_IS_SERVER,
           MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
    mg_error(c, "tls defaults %#x", -rc);
    goto fail;
  }
  mbedtls_ssl_conf_rng(&tls->conf, mbed_rng, c);
  if (opts->ca == NULL || strcmp(opts->ca, "*") == 0) {
    mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_NONE);
  }
  if (opts->ca != NULL && opts->ca[0] != '\0') {
    if (opts->crl != NULL && opts->crl[0] != '\0') {
      rc = opts->crl[0] == '-'
               ? mbedtls_x509_crl_parse(&tls->crl, (uint8_t *) opts->crl,
                                        strlen(opts->crl) + 1)
               : mbedtls_x509_crl_parse_file(&tls->crl, opts->crl);
      if (rc != 0) {
        mg_error(c, "parse(%s) err %#x", crl, -rc);
        goto fail;
      }
    }
#if defined(MBEDTLS_X509_CA_CHAIN_ON_DISK)
    tls->cafile = strdup(opts->ca);
    rc = mbedtls_ssl_conf_ca_chain_file(&tls->conf, tls->cafile, &tls->crl);
    if (rc != 0) {
      mg_error(c, "parse on-disk chain(%s) err %#x", ca, -rc);
      goto fail;
    }
#else
    rc = opts->ca[0] == '-'
             ? mbedtls_x509_crt_parse(&tls->ca, (uint8_t *) opts->ca,
                                      strlen(opts->ca) + 1)
             : mbedtls_x509_crt_parse_file(&tls->ca, opts->ca);
    if (rc != 0) {
      mg_error(c, "parse(%s) err %#x", ca, -rc);
      goto fail;
    }
    mbedtls_ssl_conf_ca_chain(&tls->conf, &tls->ca, &tls->crl);
#endif
    if (opts->srvname.len > 0) {
      char mem[128], *buf = mem;
      mg_asprintf(&buf, sizeof(mem), "%.*s", (int) opts->srvname.len,
                  opts->srvname.ptr);
      mbedtls_ssl_set_hostname(&tls->ssl, buf);
      if (buf != mem) free(buf);
    }
    mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
  }
  if (opts->cert != NULL && opts->cert[0] != '\0') {
    const char *key = opts->certkey;
    if (key == NULL) {
      key = opts->cert;
      certkey = cert;
    }
    rc = opts->cert[0] == '-'
             ? mbedtls_x509_crt_parse(&tls->cert, (uint8_t *) opts->cert,
                                      strlen(opts->cert) + 1)
             : mbedtls_x509_crt_parse_file(&tls->cert, opts->cert);
    if (rc != 0) {
      mg_error(c, "parse(%s) err %#x", cert, -rc);
      goto fail;
    }
    rc = key[0] == '-' ? mbedtls_pk_parse_key(&tls->pk, (uint8_t *) key,
                                              strlen(key) + 1, NULL, 0 RNG)
                       : mbedtls_pk_parse_keyfile(&tls->pk, key, NULL RNG);
    if (rc != 0) {
      mg_error(c, "tls key(%s) %#x", certkey, -rc);
      goto fail;
    }
    rc = mbedtls_ssl_conf_own_cert(&tls->conf, &tls->cert, &tls->pk);
    if (rc != 0) {
      mg_error(c, "own cert %#x", -rc);
      goto fail;
    }
  }
  if ((rc = mbedtls_ssl_setup(&tls->ssl, &tls->conf)) != 0) {
    mg_error(c, "setup err %#x", -rc);
    goto fail;
  }
  c->tls = tls;
  c->is_tls = 1;
  c->is_tls_hs = 1;
  if (c->is_client && c->is_resolving == 0 && c->is_connecting == 0) {
    mg_tls_handshake(c);
  }
  return;
fail:
  c->is_closing = 1;
  free(tls);
}