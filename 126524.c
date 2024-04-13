void mg_timer_free(struct mg_timer *t) {
  struct mg_timer **head = &g_timers;
  while (*head && *head != t) head = &(*head)->next;
  if (*head) *head = t->next;
}