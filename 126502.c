static bool mg_socketpair(SOCKET sp[2], union usa usa[2]) {
  socklen_t n = sizeof(usa[0].sin);
  bool result = false;

  (void) memset(&usa[0], 0, sizeof(usa[0]));
  usa[0].sin.sin_family = AF_INET;
  *(uint32_t *) &usa->sin.sin_addr = mg_htonl(0x7f000001);  // 127.0.0.1
  usa[1] = usa[0];

  if ((sp[0] = socket(AF_INET, SOCK_DGRAM, 0)) != INVALID_SOCKET &&
      (sp[1] = socket(AF_INET, SOCK_DGRAM, 0)) != INVALID_SOCKET &&
      bind(sp[0], &usa[0].sa, n) == 0 && bind(sp[1], &usa[1].sa, n) == 0 &&
      getsockname(sp[0], &usa[0].sa, &n) == 0 &&
      getsockname(sp[1], &usa[1].sa, &n) == 0 &&
      connect(sp[0], &usa[1].sa, n) == 0 &&
      connect(sp[1], &usa[0].sa, n) == 0) {
    mg_set_non_blocking_mode(sp[1]);  // Set close-on-exec
    result = true;
  } else {
    if (sp[0] != INVALID_SOCKET) closesocket(sp[0]);
    if (sp[1] != INVALID_SOCKET) closesocket(sp[1]);
    sp[0] = sp[1] = INVALID_SOCKET;
  }

  return result;
}