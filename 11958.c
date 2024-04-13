dirserv_clear_old_networkstatuses(time_t cutoff)
{
  if (!cached_v2_networkstatus)
    return;

  DIGESTMAP_FOREACH_MODIFY(cached_v2_networkstatus, id, cached_dir_t *, dir) {
    if (dir->published < cutoff) {
      char *fname;
      fname = networkstatus_get_cache_filename(id);
      if (file_status(fname) == FN_FILE) {
        log_info(LD_DIR, "Removing too-old untrusted networkstatus in %s",
                 fname);
        unlink(fname);
      }
      tor_free(fname);
      cached_dir_decref(dir);
      MAP_DEL_CURRENT(id);
    }
  } DIGESTMAP_FOREACH_END
}