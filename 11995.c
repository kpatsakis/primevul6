clear_cached_dir(cached_dir_t *d)
{
  tor_free(d->dir);
  tor_free(d->dir_z);
  memset(d, 0, sizeof(cached_dir_t));
}