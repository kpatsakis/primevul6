static void accept_conn(struct mg_mgr *mgr, struct mg_connection *lsn) {
  struct mg_connection *c = NULL;
  union usa usa;
  socklen_t sa_len = sizeof(usa);
  SOCKET fd = accept(FD(lsn), &usa.sa, &sa_len);
  if (fd == INVALID_SOCKET) {
#if MG_ARCH == MG_ARCH_AZURERTOS
    // AzureRTOS, in non-block socket mode can mark listening socket readable
    // even it is not. See comment for 'select' func implementation in nx_bsd.c
    // That's not an error, just should try later
    if (MG_SOCK_ERRNO != EAGAIN)
#endif
      LOG(LL_ERROR, ("%lu accept failed, errno %d", lsn->id, MG_SOCK_ERRNO));
#if (!defined(_WIN32) && (MG_ARCH != MG_ARCH_FREERTOS_TCP))
  } else if ((long) fd >= FD_SETSIZE) {
    LOG(LL_ERROR, ("%ld > %ld", (long) fd, (long) FD_SETSIZE));
    closesocket(fd);
#endif
  } else if ((c = alloc_conn(mgr, 0, fd)) == NULL) {
    LOG(LL_ERROR, ("%lu OOM", lsn->id));
    closesocket(fd);
  } else {
    char buf[40];
    tomgaddr(&usa, &c->peer, sa_len != sizeof(usa.sin));
    mg_straddr(&c->peer, buf, sizeof(buf));
    LOG(LL_DEBUG, ("%lu accepted %s", c->id, buf));
    mg_set_non_blocking_mode(FD(c));
    setsockopts(c);
    LIST_ADD_HEAD(struct mg_connection, &mgr->conns, c);
    c->is_accepted = 1;
    c->is_hexdumping = lsn->is_hexdumping;
    c->pfn = lsn->pfn;
    c->pfn_data = lsn->pfn_data;
    c->fn = lsn->fn;
    c->fn_data = lsn->fn_data;
    mg_call(c, MG_EV_OPEN, NULL);
    mg_call(c, MG_EV_ACCEPT, NULL);
  }
}