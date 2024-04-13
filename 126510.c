static void iolog(struct mg_connection *c, char *buf, long n, bool r) {
  LOG(n > 0 ? LL_VERBOSE_DEBUG : LL_DEBUG,
      ("%-3lu %d%d%d%d%d%d%d%d%d%d%d%d%d%d %d:%d %ld err %d", c->id,
       c->is_listening, c->is_client, c->is_accepted, c->is_resolving,
       c->is_connecting, c->is_tls, c->is_tls_hs, c->is_udp, c->is_websocket,
       c->is_hexdumping, c->is_draining, c->is_closing, c->is_readable,
       c->is_writable, (int) c->send.len, (int) c->recv.len, n, MG_SOCK_ERRNO));
  if (n == 0) {
    // Do nothing
  } else if (n < 0) {
    c->is_closing = 1;  // Error, or normal termination
  } else if (n > 0) {
    if (c->is_hexdumping) {
      union usa usa;
      char t1[50] = "", t2[50] = "";
      socklen_t slen = sizeof(usa.sin);
      char *s = mg_hexdump(buf, (size_t) n);
      struct mg_addr a;
      memset(&usa, 0, sizeof(usa));
      memset(&a, 0, sizeof(a));
      getsockname(FD(c), &usa.sa, &slen);
      tomgaddr(&usa, &a, c->peer.is_ip6);
      LOG(LL_INFO, ("\n-- %lu %s %s %s %s %ld\n%s", c->id,
                    mg_straddr(&a, t1, sizeof(t1)), r ? "<-" : "->",
                    mg_straddr(&c->peer, t2, sizeof(t2)), c->label, n, s));
      free(s);
      (void) t1, (void) t2;  // Silence warnings for MG_ENABLE_LOG=0
    }
    if (r) {
      struct mg_str evd = mg_str_n(buf, (size_t) n);
      c->recv.len += (size_t) n;
      mg_call(c, MG_EV_READ, &evd);
    } else {
      mg_iobuf_del(&c->send, 0, (size_t) n);
      if (c->send.len == 0) mg_iobuf_resize(&c->send, 0);
      mg_call(c, MG_EV_WRITE, &n);
    }
  }
}