static void mg_iotest(struct mg_mgr *mgr, int ms) {
#if MG_ARCH == MG_ARCH_FREERTOS_TCP
  struct mg_connection *c;
  for (c = mgr->conns; c != NULL; c = c->next) {
    if (c->is_closing || c->is_resolving || FD(c) == INVALID_SOCKET) continue;
    FreeRTOS_FD_SET(c->fd, mgr->ss, eSELECT_READ | eSELECT_EXCEPT);
    if (c->is_connecting || (c->send.len > 0 && c->is_tls_hs == 0))
      FreeRTOS_FD_SET(c->fd, mgr->ss, eSELECT_WRITE);
  }
  FreeRTOS_select(mgr->ss, pdMS_TO_TICKS(ms));
  for (c = mgr->conns; c != NULL; c = c->next) {
    EventBits_t bits = FreeRTOS_FD_ISSET(c->fd, mgr->ss);
    c->is_readable = bits & (eSELECT_READ | eSELECT_EXCEPT) ? 1 : 0;
    c->is_writable = bits & eSELECT_WRITE ? 1 : 0;
    FreeRTOS_FD_CLR(c->fd, mgr->ss,
                    eSELECT_READ | eSELECT_EXCEPT | eSELECT_WRITE);
  }
#else
  struct timeval tv = {ms / 1000, (ms % 1000) * 1000};
  struct mg_connection *c;
  fd_set rset, wset;
  SOCKET maxfd = 0;
  int rc;

  FD_ZERO(&rset);
  FD_ZERO(&wset);

  for (c = mgr->conns; c != NULL; c = c->next) {
    if (c->is_closing || c->is_resolving || FD(c) == INVALID_SOCKET) continue;
    FD_SET(FD(c), &rset);
    if (FD(c) > maxfd) maxfd = FD(c);
    if (c->is_connecting || (c->send.len > 0 && c->is_tls_hs == 0))
      FD_SET(FD(c), &wset);
  }

  if ((rc = select((int) maxfd + 1, &rset, &wset, NULL, &tv)) < 0) {
    LOG(LL_DEBUG, ("select: %d %d", rc, MG_SOCK_ERRNO));
    FD_ZERO(&rset);
    FD_ZERO(&wset);
  }

  for (c = mgr->conns; c != NULL; c = c->next) {
    // TLS might have stuff buffered, so dig everything
    c->is_readable = FD(c) != INVALID_SOCKET && FD_ISSET(FD(c), &rset);
    c->is_writable = FD(c) != INVALID_SOCKET && FD_ISSET(FD(c), &wset);
  }
#endif
}