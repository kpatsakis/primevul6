static void setsockopts(struct mg_connection *c) {
#if MG_ARCH == MG_ARCH_FREERTOS_TCP || MG_ARCH == MG_ARCH_AZURERTOS
  (void) c;
#else
  int on = 1;
#if !defined(SOL_TCP)
#define SOL_TCP IPPROTO_TCP
#endif
  setsockopt(FD(c), SOL_TCP, TCP_NODELAY, (char *) &on, sizeof(on));
#if defined(TCP_QUICKACK)
  setsockopt(FD(c), SOL_TCP, TCP_QUICKACK, (char *) &on, sizeof(on));
#endif
  setsockopt(FD(c), SOL_SOCKET, SO_KEEPALIVE, (char *) &on, sizeof(on));
#if (defined(ESP32) && ESP32) || (defined(ESP8266) && ESP8266) || \
    defined(__linux__)
  int idle = 60;
  setsockopt(FD(c), IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle));
#endif
#if !defined(_WIN32) && !defined(__QNX__)
  {
    int cnt = 3, intvl = 20;
    setsockopt(FD(c), IPPROTO_TCP, TCP_KEEPCNT, &cnt, sizeof(cnt));
    setsockopt(FD(c), IPPROTO_TCP, TCP_KEEPINTVL, &intvl, sizeof(intvl));
  }
#endif
#endif
}