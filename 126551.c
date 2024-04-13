static SOCKET mg_open_listener(const char *url, struct mg_addr *addr) {
  SOCKET fd = INVALID_SOCKET;
  int s_err = 0;  // Memoized socket error, in case closesocket() overrides it
  memset(addr, 0, sizeof(*addr));
  addr->port = mg_htons(mg_url_port(url));
  if (!mg_aton(mg_url_host(url), addr)) {
    LOG(LL_ERROR, ("invalid listening URL: %s", url));
  } else {
    union usa usa;
    socklen_t slen = tousa(addr, &usa);
    int on = 1, af = addr->is_ip6 ? AF_INET6 : AF_INET;
    int type = strncmp(url, "udp:", 4) == 0 ? SOCK_DGRAM : SOCK_STREAM;
    int proto = type == SOCK_DGRAM ? IPPROTO_UDP : IPPROTO_TCP;
    (void) on;

    if ((fd = socket(af, type, proto)) != INVALID_SOCKET &&
#if (!defined(_WIN32) || !defined(SO_EXCLUSIVEADDRUSE)) && \
    (!defined(LWIP_SOCKET) || (defined(LWIP_SOCKET) && SO_REUSE == 1))
        // 1. SO_RESUSEADDR is not enabled on Windows because the semantics of
        //    SO_REUSEADDR on UNIX and Windows is different. On Windows,
        //    SO_REUSEADDR allows to bind a socket to a port without error even
        //    if the port is already open by another program. This is not the
        //    behavior SO_REUSEADDR was designed for, and leads to hard-to-track
        //    failure scenarios. Therefore, SO_REUSEADDR was disabled on Windows
        //    unless SO_EXCLUSIVEADDRUSE is supported and set on a socket.
        // 2. In case of LWIP, SO_REUSEADDR should be explicitly enabled, by
        // defining
        //    SO_REUSE (in lwipopts.h), otherwise the code below will compile
        //    but won't work! (setsockopt will return EINVAL)
        !setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on)) &&
#endif
#if defined(_WIN32) && defined(SO_EXCLUSIVEADDRUSE) && !defined(WINCE)
        // "Using SO_REUSEADDR and SO_EXCLUSIVEADDRUSE"
        //! setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (char *) &on, sizeof(on))
        //! &&
        !setsockopt(fd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *) &on,
                    sizeof(on)) &&
#endif
        bind(fd, &usa.sa, slen) == 0 &&
        // NOTE(lsm): FreeRTOS uses backlog value as a connection limit
        (type == SOCK_DGRAM || listen(fd, MG_SOCK_LISTEN_BACKLOG_SIZE) == 0)) {
      // In case port was set to 0, get the real port number
      if (getsockname(fd, &usa.sa, &slen) == 0) {
        addr->port = usa.sin.sin_port;
#if MG_ENABLE_IPV6
        if (addr->is_ip6) addr->port = usa.sin6.sin6_port;
#endif
      }
      mg_set_non_blocking_mode(fd);
    } else if (fd != INVALID_SOCKET) {
      s_err = MG_SOCK_ERRNO;
      closesocket(fd);
      fd = INVALID_SOCKET;
    }
  }
  if (fd == INVALID_SOCKET) {
    if (s_err == 0) s_err = MG_SOCK_ERRNO;
    LOG(LL_ERROR, ("Failed to listen on %s, errno %d", url, s_err));
  }

  return fd;
}