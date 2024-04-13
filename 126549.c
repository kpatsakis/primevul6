void mg_sntp_send(struct mg_connection *c, unsigned long utc) {
  if (c->is_resolving) {
    LOG(LL_ERROR, ("%lu wait until resolved", c->id));
  } else if (utc > s_sntmp_next) {
    uint8_t buf[48] = {0};
    s_sntmp_next = utc + SNTP_INTERVAL_SEC;
    buf[0] = (0 << 6) | (4 << 3) | 3;
    mg_send(c, buf, sizeof(buf));
    LOG(LL_DEBUG, ("%lu ct %lu, next at %lu", c->id, utc, s_sntmp_next));
  }
}