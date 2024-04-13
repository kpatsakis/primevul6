void mg_resolve(struct mg_connection *c, const char *url) {
  struct mg_str host = mg_url_host(url);
  c->peer.port = mg_htons(mg_url_port(url));
  if (mg_aton(host, &c->peer)) {
    // host is an IP address, do not fire name resolution
    mg_connect_resolved(c);
  } else {
    // host is not an IP, send DNS resolution request
    mg_sendnsreq(c, &host, c->mgr->dnstimeout, &c->mgr->dns4, false);
  }
}