dirserv_pick_cached_dir_obj(cached_dir_t *cache_src,
                            cached_dir_t *auth_src,
                            time_t dirty, cached_dir_t *(*regenerate)(void),
                            const char *name,
                            authority_type_t auth_type)
{
  or_options_t *options = get_options();
  int authority = (auth_type == V1_AUTHORITY && authdir_mode_v1(options)) ||
                  (auth_type == V2_AUTHORITY && authdir_mode_v2(options));

  if (!authority || authdir_mode_bridge(options)) {
    return cache_src;
  } else {
    /* We're authoritative. */
    if (regenerate != NULL) {
      if (dirty && dirty + DIR_REGEN_SLACK_TIME < time(NULL)) {
        if (!(auth_src = regenerate())) {
          log_err(LD_BUG, "Couldn't generate %s?", name);
          exit(1);
        }
      } else {
        log_info(LD_DIRSERV, "The %s is still clean; reusing.", name);
      }
    }
    return auth_src ? auth_src : cache_src;
  }
}