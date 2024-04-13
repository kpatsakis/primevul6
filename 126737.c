void mg_connect_resolved(struct mg_connection *c) {
  // char buf[40];
  int type = c->is_udp ? SOCK_DGRAM : SOCK_STREAM;
  int rc, af = c->peer.is_ip6 ? AF_INET6 : AF_INET;
  // mg_straddr(&c->peer, buf, sizeof(buf));
  c->fd = S2PTR(socket(af, type, 0));
  if (FD(c) == INVALID_SOCKET) {
    mg_error(c, "socket(): %d", MG_SOCK_ERRNO);
  } else if (c->is_udp) {
    mg_call(c, MG_EV_RESOLVE, NULL);
    mg_call(c, MG_EV_CONNECT, NULL);
  } else {
    union usa usa;
    socklen_t slen = tousa(&c->peer, &usa);
    mg_set_non_blocking_mode(FD(c));
    setsockopts(c);
    mg_call(c, MG_EV_RESOLVE, NULL);
    if ((rc = connect(FD(c), &usa.sa, slen)) == 0) {
      mg_call(c, MG_EV_CONNECT, NULL);
    } else if (mg_sock_would_block()) {
      c->is_connecting = 1;
    } else {
      mg_error(c, "connect: %d", MG_SOCK_ERRNO);
    }
  }
}