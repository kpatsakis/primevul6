int64_t mg_sntp_parse(const unsigned char *buf, size_t len) {
  int64_t res = -1;
  int mode = len > 0 ? buf[0] & 7 : 0;
  if (len < 48) {
    LOG(LL_ERROR, ("%s", "corrupt packet"));
  } else if ((buf[0] & 0x38) >> 3 != 4) {
    LOG(LL_ERROR, ("%s", "wrong version"));
  } else if (mode != 4 && mode != 5) {
    LOG(LL_ERROR, ("%s", "not a server reply"));
  } else if (buf[1] == 0) {
    LOG(LL_ERROR, ("%s", "server sent a kiss of death"));
  } else {
    uint32_t *data = (uint32_t *) &buf[40];
    unsigned long seconds = mg_ntohl(data[0]) - SNTP_TIME_OFFSET;
    unsigned long useconds = mg_ntohl(data[1]);
    // LOG(LL_DEBUG, ("%lu %lu %lu", time(0), seconds, useconds));
    res = ((int64_t) seconds) * 1000 + (int64_t) ((useconds / 1000) % 1000);
    s_sntmp_next = seconds + SNTP_INTERVAL_SEC;
  }
  return res;
}