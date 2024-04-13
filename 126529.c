static long mg_sock_send(struct mg_connection *c, const void *buf, size_t len) {
  long n;
  if (c->is_udp) {
    union usa usa;
    socklen_t slen = tousa(&c->peer, &usa);
    n = sendto(FD(c), (char *) buf, len, 0, &usa.sa, slen);
  } else {
    n = send(FD(c), (char *) buf, len, MSG_NONBLOCKING);
  }
  return n == 0 ? -1 : n < 0 && mg_sock_would_block() ? 0 : n;
}