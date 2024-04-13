set_cached_dir(cached_dir_t *d, char *directory, time_t when)
{
  time_t now = time(NULL);
  if (when<=d->published) {
    log_info(LD_DIRSERV, "Ignoring old directory; not caching.");
    tor_free(directory);
  } else if (when>=now+ROUTER_MAX_AGE_TO_PUBLISH) {
    log_info(LD_DIRSERV, "Ignoring future directory; not caching.");
    tor_free(directory);
  } else {
    /* if (when>d->published && when<now+ROUTER_MAX_AGE) */
    log_debug(LD_DIRSERV, "Caching directory.");
    tor_free(d->dir);
    d->dir = directory;
    d->dir_len = strlen(directory);
    tor_free(d->dir_z);
    if (tor_gzip_compress(&(d->dir_z), &(d->dir_z_len), d->dir, d->dir_len,
                          ZLIB_METHOD)) {
      log_warn(LD_BUG,"Error compressing cached directory");
    }
    d->published = when;
  }
}