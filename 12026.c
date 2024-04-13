dirserv_thinks_router_is_hs_dir(routerinfo_t *router, time_t now)
{

  long uptime;

  /* If we haven't been running for at least
   * get_options()->MinUptimeHidServDirectoryV2 seconds, we can't
   * have accurate data telling us a relay has been up for at least
   * that long. We also want to allow a bit of slack: Reachability
   * tests aren't instant. If we haven't been running long enough,
   * trust the relay. */

  if (stats_n_seconds_working >
      get_options()->MinUptimeHidServDirectoryV2 * 1.1)
    uptime = MIN(rep_hist_get_uptime(router->cache_info.identity_digest, now),
                 real_uptime(router, now));
  else
    uptime = real_uptime(router, now);

  /* XXX We shouldn't need to check dir_port, but we do because of
   * bug 1693. In the future, once relays set wants_to_be_hs_dir
   * correctly, we can revert to only checking dir_port if router's
   * version is too old. */
  /* XXX Unfortunately, we need to keep checking dir_port until all
   * *clients* suffering from bug 2722 are obsolete.  The first version
   * to fix the bug was 0.2.2.25-alpha. */
  return (router->wants_to_be_hs_dir && router->dir_port &&
          uptime > get_options()->MinUptimeHidServDirectoryV2 &&
          router->is_running);
}