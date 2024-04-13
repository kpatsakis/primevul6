void mg_mgr_free(struct mg_mgr *mgr) {
  struct mg_connection *c;
  for (c = mgr->conns; c != NULL; c = c->next) c->is_closing = 1;
  mg_mgr_poll(mgr, 0);
#if MG_ARCH == MG_ARCH_FREERTOS_TCP
  FreeRTOS_DeleteSocketSet(mgr->ss);
#endif
  LOG(LL_INFO, ("All connections closed"));
}