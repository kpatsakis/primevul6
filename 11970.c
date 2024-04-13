dirserv_set_cached_networkstatus_v2(const char *networkstatus,
                                    const char *identity,
                                    time_t published)
{
  cached_dir_t *d, *old_d;
  smartlist_t *trusted_dirs;
  if (!cached_v2_networkstatus)
    cached_v2_networkstatus = digestmap_new();

  old_d = digestmap_get(cached_v2_networkstatus, identity);
  if (!old_d && !networkstatus)
    return;

  if (networkstatus) {
    if (!old_d || published > old_d->published) {
      d = new_cached_dir(tor_strdup(networkstatus), published);
      digestmap_set(cached_v2_networkstatus, identity, d);
      if (old_d)
        cached_dir_decref(old_d);
    }
  } else {
    if (old_d) {
      digestmap_remove(cached_v2_networkstatus, identity);
      cached_dir_decref(old_d);
    }
  }

  /* Now purge old entries. */
  trusted_dirs = router_get_trusted_dir_servers();
  if (digestmap_size(cached_v2_networkstatus) >
      smartlist_len(trusted_dirs) + MAX_UNTRUSTED_NETWORKSTATUSES) {
    /* We need to remove the oldest untrusted networkstatus. */
    const char *oldest = NULL;
    time_t oldest_published = TIME_MAX;
    digestmap_iter_t *iter;

    for (iter = digestmap_iter_init(cached_v2_networkstatus);
         !digestmap_iter_done(iter);
         iter = digestmap_iter_next(cached_v2_networkstatus, iter)) {
      const char *ident;
      void *val;
      digestmap_iter_get(iter, &ident, &val);
      d = val;
      if (d->published < oldest_published &&
          !router_digest_is_trusted_dir(ident)) {
        oldest = ident;
        oldest_published = d->published;
      }
    }
    tor_assert(oldest);
    d = digestmap_remove(cached_v2_networkstatus, oldest);
    if (d)
      cached_dir_decref(d);
  }
}