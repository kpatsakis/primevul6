dirserv_set_router_is_running(routerinfo_t *router, time_t now)
{
  /*XXXX023 This function is a mess.  Separate out the part that calculates
    whether it's reachable and the part that tells rephist that the router was
    unreachable.
   */
  int answer;

  if (router_is_me(router)) {
    /* We always know if we are down ourselves. */
    answer = ! we_are_hibernating();
  } else if (router->is_hibernating &&
             (router->cache_info.published_on +
              HIBERNATION_PUBLICATION_SKEW) > router->last_reachable) {
    /* A hibernating router is down unless we (somehow) had contact with it
     * since it declared itself to be hibernating. */
    answer = 0;
  } else if (get_options()->AssumeReachable) {
    /* If AssumeReachable, everybody is up unless they say they are down! */
    answer = 1;
  } else {
    /* Otherwise, a router counts as up if we found it reachable in the last
       REACHABLE_TIMEOUT seconds. */
    answer = (now < router->last_reachable + REACHABLE_TIMEOUT);
  }

  if (!answer && running_long_enough_to_decide_unreachable()) {
    /* Not considered reachable. tell rephist about that.

       Because we launch a reachability test for each router every
       REACHABILITY_TEST_CYCLE_PERIOD seconds, then the router has probably
       been down since at least that time after we last successfully reached
       it.
     */
    time_t when = now;
    if (router->last_reachable &&
        router->last_reachable + REACHABILITY_TEST_CYCLE_PERIOD < now)
      when = router->last_reachable + REACHABILITY_TEST_CYCLE_PERIOD;
    rep_hist_note_router_unreachable(router->cache_info.identity_digest, when);
  }

  router->is_running = answer;
}