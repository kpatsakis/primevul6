void mg_tls_free(struct mg_connection *c) {
  struct mg_tls *tls = (struct mg_tls *) c->tls;
  if (tls == NULL) return;
  free(tls->cafile);
  mbedtls_ssl_free(&tls->ssl);
  mbedtls_pk_free(&tls->pk);
  mbedtls_x509_crt_free(&tls->ca);
  mbedtls_x509_crl_free(&tls->crl);
  mbedtls_x509_crt_free(&tls->cert);
  mbedtls_ssl_config_free(&tls->conf);
  free(tls);
  c->tls = NULL;
}