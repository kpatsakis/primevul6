static void connect_conn(struct mg_connection *c) {
  int rc = 0;
#if MG_ARCH != MG_ARCH_FREERTOS_TCP
  socklen_t len = sizeof(rc);
  if (getsockopt(FD(c), SOL_SOCKET, SO_ERROR, (char *) &rc, &len)) rc = 1;
#endif
  if (rc == EAGAIN || rc == EWOULDBLOCK) rc = 0;
  c->is_connecting = 0;
  if (rc) {
    char buf[50];
    mg_error(c, "error connecting to %s",
             mg_straddr(&c->peer, buf, sizeof(buf)));
  } else {
    if (c->is_tls_hs) mg_tls_handshake(c);
    mg_call(c, MG_EV_CONNECT, NULL);
  }
}