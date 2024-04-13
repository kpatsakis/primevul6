static void debug_cb(void *c, int lev, const char *s, int n, const char *s2) {
  n = (int) strlen(s2) - 1;
  LOG(LL_VERBOSE_DEBUG, ("%p %.*s", ((struct mg_connection *) c)->fd, n, s2));
  (void) s;
  (void) c;
  (void) lev;
}