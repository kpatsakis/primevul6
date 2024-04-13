static void close_conn(struct mg_connection *c) {
  mg_resolve_cancel(c);  // Close any pending DNS query
  LIST_DELETE(struct mg_connection, &c->mgr->conns, c);
  if (c == c->mgr->dns4.c) c->mgr->dns4.c = NULL;
  if (c == c->mgr->dns6.c) c->mgr->dns6.c = NULL;
  // Order of operations is important. `MG_EV_CLOSE` event must be fired
  // before we deallocate received data, see #1331
  mg_call(c, MG_EV_CLOSE, NULL);
  LOG(LL_DEBUG, ("%lu closed", c->id));
  if (FD(c) != INVALID_SOCKET) {
    closesocket(FD(c));
#if MG_ARCH == MG_ARCH_FREERTOS_TCP
    FreeRTOS_FD_CLR(c->fd, c->mgr->ss, eSELECT_ALL);
#endif
    c->fd = S2PTR(INVALID_SOCKET);
  }
  mg_tls_free(c);
  mg_iobuf_free(&c->recv);
  mg_iobuf_free(&c->send);
  memset(c, 0, sizeof(*c));
  free(c);
}