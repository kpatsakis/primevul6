static void mg_sendnsreq(struct mg_connection *c, struct mg_str *name, int ms,
                         struct mg_dns *dnsc, bool ipv6) {
  struct dns_data *d = NULL;
  if (dnsc->url == NULL) {
    mg_error(c, "DNS server URL is NULL. Call mg_mgr_init()");
  } else if (dnsc->c == NULL) {
    dnsc->c = mg_connect(c->mgr, dnsc->url, NULL, NULL);
    if (dnsc->c != NULL) {
      dnsc->c->pfn = dns_cb;
      // snprintf(dnsc->c->label, sizeof(dnsc->c->label), "%s", "DNS");
      // dnsc->c->is_hexdumping = 1;
    }
  }
  if (dnsc->c == NULL) {
    mg_error(c, "resolver");
  } else if ((d = (struct dns_data *) calloc(1, sizeof(*d))) == NULL) {
    mg_error(c, "resolve OOM");
  } else {
#if MG_ENABLE_LOG
    char buf[100];
#endif
    d->txnid = s_reqs ? (uint16_t) (s_reqs->txnid + 1) : 1;
    d->next = s_reqs;
    s_reqs = d;
    d->expire = mg_millis() + (int64_t) ms;
    d->c = c;
    c->is_resolving = 1;
    LOG(LL_VERBOSE_DEBUG,
        ("%lu resolving %.*s @ %s, txnid %hu", c->id, (int) name->len,
         name->ptr, mg_ntoa(&dnsc->c->peer, buf, sizeof(buf)), d->txnid));
    mg_dns_send(dnsc->c, name, d->txnid, ipv6);
  }
}