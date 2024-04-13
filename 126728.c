void mg_timer_init(struct mg_timer *t, int64_t ms, unsigned flags,
                   void (*fn)(void *), void *arg) {
  struct mg_timer tmp = {ms, 0UL, flags, fn, arg, g_timers};
  *t = tmp;
  g_timers = t;
  if (flags & MG_TIMER_RUN_NOW) fn(arg);
}