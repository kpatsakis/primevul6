dirserv_orconn_tls_done(const char *address,
                        uint16_t or_port,
                        const char *digest_rcvd,
                        int as_advertised)
{
  routerlist_t *rl = router_get_routerlist();
  time_t now = time(NULL);
  int bridge_auth = authdir_mode_bridge(get_options());
  tor_assert(address);
  tor_assert(digest_rcvd);

  /* XXX023 Doing a loop like this is stupid.  We should just look up the
   * router by digest_rcvd, and see if address, orport, and as_advertised
   * match up. -NM */
  SMARTLIST_FOREACH_BEGIN(rl->routers, routerinfo_t *, ri) {
    if (!strcasecmp(address, ri->address) && or_port == ri->or_port &&
        as_advertised &&
        fast_memeq(ri->cache_info.identity_digest, digest_rcvd, DIGEST_LEN)) {
      /* correct digest. mark this router reachable! */
      if (!bridge_auth || ri->purpose == ROUTER_PURPOSE_BRIDGE) {
        tor_addr_t addr, *addrp=NULL;
        log_info(LD_DIRSERV, "Found router %s to be reachable at %s:%d. Yay.",
                 router_describe(ri),
                 address, ri->or_port);
        if (tor_addr_from_str(&addr, ri->address) != -1)
          addrp = &addr;
        else
          log_warn(LD_BUG, "Couldn't parse IP address \"%s\"", ri->address);
        rep_hist_note_router_reachable(digest_rcvd, addrp, or_port, now);
        ri->last_reachable = now;
      }
    }
  } SMARTLIST_FOREACH_END(ri);
  /* FFFF Maybe we should reinstate the code that dumps routers with the same
   * addr/port but with nonmatching keys, but instead of dumping, we should
   * skip testing. */
}