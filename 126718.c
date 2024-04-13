static long mg_sock_recv(struct mg_connection *c, void *buf, size_t len) {
  long n = 0;
  if (c->is_udp) {
    union usa usa;
    socklen_t slen = tousa(&c->peer, &usa);
    n = recvfrom(FD(c), (char *) buf, len, 0, &usa.sa, &slen);
    if (n > 0) tomgaddr(&usa, &c->peer, slen != sizeof(usa.sin));
  } else {
    n = recv(FD(c), (char *) buf, len, MSG_NONBLOCKING);
  }
  return n == 0 ? -1 : n < 0 && mg_sock_would_block() ? 0 : n;
}