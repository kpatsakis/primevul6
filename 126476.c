void mg_mgr_init(struct mg_mgr *mgr) {
  memset(mgr, 0, sizeof(*mgr));
#if defined(_WIN32) && MG_ENABLE_WINSOCK
  // clang-format off
  { WSADATA data; WSAStartup(MAKEWORD(2, 2), &data); }
  // clang-format on
#elif MG_ARCH == MG_ARCH_FREERTOS_TCP
  mgr->ss = FreeRTOS_CreateSocketSet();
#elif defined(__unix) || defined(__unix__) || defined(__APPLE__)
  // Ignore SIGPIPE signal, so if client cancels the request, it
  // won't kill the whole process.
  signal(SIGPIPE, SIG_IGN);
#endif
  mgr->dnstimeout = 3000;
  mgr->dns4.url = "udp://8.8.8.8:53";
  mgr->dns6.url = "udp://[2001:4860:4860::8888]:53";
}