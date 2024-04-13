static bool mg_sock_would_block(void) {
  int err = MG_SOCK_ERRNO;
  return err == EINPROGRESS || err == EWOULDBLOCK
#ifndef WINCE
         || err == EAGAIN || err == EINTR
#endif
#if defined(_WIN32) && MG_ENABLE_WINSOCK
         || err == WSAEINTR || err == WSAEWOULDBLOCK
#endif
      ;
}