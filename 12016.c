dirserv_add_extrainfo(extrainfo_t *ei, const char **msg)
{
  routerinfo_t *ri;
  int r;
  tor_assert(msg);
  *msg = NULL;

  ri = router_get_by_digest(ei->cache_info.identity_digest);
  if (!ri) {
    *msg = "No corresponding router descriptor for extra-info descriptor";
    extrainfo_free(ei);
    return ROUTER_BAD_EI;
  }

  /* If it's too big, refuse it now. Otherwise we'll cache it all over the
   * network and it'll clog everything up. */
  if (ei->cache_info.signed_descriptor_len > MAX_EXTRAINFO_UPLOAD_SIZE) {
    log_notice(LD_DIR, "Somebody attempted to publish an extrainfo "
               "with size %d. Either this is an attack, or the "
               "MAX_EXTRAINFO_UPLOAD_SIZE (%d) constant is too low.",
               (int)ei->cache_info.signed_descriptor_len,
               MAX_EXTRAINFO_UPLOAD_SIZE);
    *msg = "Extrainfo document was too large";
    extrainfo_free(ei);
    return ROUTER_BAD_EI;
  }

  if ((r = routerinfo_incompatible_with_extrainfo(ri, ei, NULL, msg))) {
    extrainfo_free(ei);
    return r < 0 ? ROUTER_WAS_NOT_NEW : ROUTER_BAD_EI;
  }
  router_add_extrainfo_to_routerlist(ei, msg, 0, 0);
  return ROUTER_ADDED_SUCCESSFULLY;
}