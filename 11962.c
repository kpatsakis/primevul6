dirserv_test_reachability(time_t now)
{
  /* XXX decide what to do here; see or-talk thread "purging old router
   * information, revocation." -NM
   * We can't afford to mess with this in 0.1.2.x. The reason is that
   * if we stop doing reachability tests on some of routerlist, then
   * we'll for-sure think they're down, which may have unexpected
   * effects in other parts of the code. It doesn't hurt much to do
   * the testing, and directory authorities are easy to upgrade. Let's
   * wait til 0.2.0. -RD */
//  time_t cutoff = now - ROUTER_MAX_AGE_TO_PUBLISH;
  routerlist_t *rl = router_get_routerlist();
  static char ctr = 0;
  int bridge_auth = authdir_mode_bridge(get_options());

  SMARTLIST_FOREACH_BEGIN(rl->routers, routerinfo_t *, router) {
    const char *id_digest = router->cache_info.identity_digest;
    if (router_is_me(router))
      continue;
    if (bridge_auth && router->purpose != ROUTER_PURPOSE_BRIDGE)
      continue; /* bridge authorities only test reachability on bridges */
//    if (router->cache_info.published_on > cutoff)
//      continue;
    if ((((uint8_t)id_digest[0]) % REACHABILITY_MODULO_PER_TEST) == ctr) {
      dirserv_single_reachability_test(now, router);
    }
  } SMARTLIST_FOREACH_END(router);
  ctr = (ctr + 1) % REACHABILITY_MODULO_PER_TEST; /* increment ctr */
}