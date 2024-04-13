static void test_timer(void) {
  int v1 = 0, v2 = 0, v3 = 0;
  struct mg_timer t1, t2, t3;

  LOG(LL_INFO, ("g_timers: %p", g_timers));
  ASSERT(g_timers == NULL);

  mg_timer_init(&t1, 5, MG_TIMER_REPEAT, f1, &v1);
  mg_timer_init(&t2, 15, 0, f1, &v2);
  mg_timer_init(&t3, 10, MG_TIMER_REPEAT | MG_TIMER_RUN_NOW, f1, &v3);

  ASSERT(g_timers == &t3);
  ASSERT(g_timers->next == &t2);

  mg_timer_poll(0);
  mg_timer_poll(1);
  ASSERT(v1 == 0);
  ASSERT(v2 == 0);
  ASSERT(v3 == 1);

  mg_timer_poll(5);
  ASSERT(v1 == 1);
  ASSERT(v2 == 0);
  ASSERT(v3 == 1);

  ASSERT(g_timers == &t3);
  ASSERT(g_timers->next == &t2);

  // Simulate long delay - timers must invalidate expiration times
  mg_timer_poll(100);
  ASSERT(v1 == 2);
  ASSERT(v2 == 1);
  ASSERT(v3 == 2);

  ASSERT(g_timers == &t3);
  ASSERT(g_timers->next == &t1);  // t2 should be removed
  ASSERT(g_timers->next->next == NULL);

  mg_timer_poll(107);
  ASSERT(v1 == 3);
  ASSERT(v2 == 1);
  ASSERT(v3 == 2);

  mg_timer_poll(114);
  ASSERT(v1 == 4);
  ASSERT(v2 == 1);
  ASSERT(v3 == 3);

  mg_timer_poll(115);
  ASSERT(v1 == 5);
  ASSERT(v2 == 1);
  ASSERT(v3 == 3);

  mg_timer_init(&t2, 3, 0, f1, &v2);
  ASSERT(g_timers == &t2);
  ASSERT(g_timers->next == &t3);
  ASSERT(g_timers->next->next == &t1);
  ASSERT(g_timers->next->next->next == NULL);

  mg_timer_poll(120);
  ASSERT(v1 == 6);
  ASSERT(v2 == 1);
  ASSERT(v3 == 4);

  mg_timer_poll(125);
  ASSERT(v1 == 7);
  ASSERT(v2 == 2);
  ASSERT(v3 == 4);

  // Test millisecond counter wrap - when time goes back.
  mg_timer_poll(0);
  ASSERT(v1 == 7);
  ASSERT(v2 == 2);
  ASSERT(v3 == 4);

  ASSERT(g_timers == &t3);
  ASSERT(g_timers->next == &t1);
  ASSERT(g_timers->next->next == NULL);

  mg_timer_poll(7);
  ASSERT(v1 == 8);
  ASSERT(v2 == 2);
  ASSERT(v3 == 4);

  mg_timer_poll(11);
  ASSERT(v1 == 9);
  ASSERT(v2 == 2);
  ASSERT(v3 == 5);

  mg_timer_free(&t1);
  ASSERT(g_timers == &t3);
  ASSERT(g_timers->next == NULL);

  mg_timer_free(&t2);
  ASSERT(g_timers == &t3);
  ASSERT(g_timers->next == NULL);

  mg_timer_free(&t3);
  ASSERT(g_timers == NULL);
}