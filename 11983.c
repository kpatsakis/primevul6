dirserv_add_descriptor(routerinfo_t *ri, const char **msg, const char *source)
{
  was_router_added_t r;
  routerinfo_t *ri_old;
  char *desc, *nickname;
  size_t desclen = 0;
  *msg = NULL;

  /* If it's too big, refuse it now. Otherwise we'll cache it all over the
   * network and it'll clog everything up. */
  if (ri->cache_info.signed_descriptor_len > MAX_DESCRIPTOR_UPLOAD_SIZE) {
    log_notice(LD_DIR, "Somebody attempted to publish a router descriptor '%s'"
               " (source: %s) with size %d. Either this is an attack, or the "
               "MAX_DESCRIPTOR_UPLOAD_SIZE (%d) constant is too low.",
               ri->nickname, source, (int)ri->cache_info.signed_descriptor_len,
               MAX_DESCRIPTOR_UPLOAD_SIZE);
    *msg = "Router descriptor was too large";
    control_event_or_authdir_new_descriptor("REJECTED",
               ri->cache_info.signed_descriptor_body,
               ri->cache_info.signed_descriptor_len, *msg);
    routerinfo_free(ri);
    return ROUTER_AUTHDIR_REJECTS;
  }

  /* Check whether this descriptor is semantically identical to the last one
   * from this server.  (We do this here and not in router_add_to_routerlist
   * because we want to be able to accept the newest router descriptor that
   * another authority has, so we all converge on the same one.) */
  ri_old = router_get_by_digest(ri->cache_info.identity_digest);
  if (ri_old && ri_old->cache_info.published_on < ri->cache_info.published_on
      && router_differences_are_cosmetic(ri_old, ri)
      && !router_is_me(ri)) {
    log_info(LD_DIRSERV,
             "Not replacing descriptor from %s (source: %s); "
             "differences are cosmetic.",
             router_describe(ri), source);
    *msg = "Not replacing router descriptor; no information has changed since "
      "the last one with this identity.";
    control_event_or_authdir_new_descriptor("DROPPED",
                         ri->cache_info.signed_descriptor_body,
                         ri->cache_info.signed_descriptor_len, *msg);
    routerinfo_free(ri);
    return ROUTER_WAS_NOT_NEW;
  }

  /* Make a copy of desc, since router_add_to_routerlist might free
   * ri and its associated signed_descriptor_t. */
  desclen = ri->cache_info.signed_descriptor_len;
  desc = tor_strndup(ri->cache_info.signed_descriptor_body, desclen);
  nickname = tor_strdup(ri->nickname);

  /* Tell if we're about to need to launch a test if we add this. */
  ri->needs_retest_if_added =
    dirserv_should_launch_reachability_test(ri, ri_old);

  r = router_add_to_routerlist(ri, msg, 0, 0);
  if (!WRA_WAS_ADDED(r)) {
    /* unless the routerinfo was fine, just out-of-date */
    if (WRA_WAS_REJECTED(r))
      control_event_or_authdir_new_descriptor("REJECTED", desc, desclen, *msg);
    log_info(LD_DIRSERV,
             "Did not add descriptor from '%s' (source: %s): %s.",
             nickname, source, *msg ? *msg : "(no message)");
  } else {
    smartlist_t *changed;
    control_event_or_authdir_new_descriptor("ACCEPTED", desc, desclen, *msg);

    changed = smartlist_create();
    smartlist_add(changed, ri);
    routerlist_descriptors_added(changed, 0);
    smartlist_free(changed);
    if (!*msg) {
      *msg =  ri->is_valid ? "Descriptor for valid server accepted" :
        "Descriptor for invalid server accepted";
    }
    log_info(LD_DIRSERV,
             "Added descriptor from '%s' (source: %s): %s.",
             nickname, source, *msg);
  }
  tor_free(desc);
  tor_free(nickname);
  return r;
}