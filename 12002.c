dirserv_regenerate_directory(void)
{
  char *new_directory=NULL;

  if (dirserv_dump_directory_to_string(&new_directory,
                                       get_server_identity_key())) {
    log_warn(LD_BUG, "Error creating directory.");
    tor_free(new_directory);
    return NULL;
  }
  cached_dir_decref(the_directory);
  the_directory = new_cached_dir(new_directory, time(NULL));
  log_info(LD_DIRSERV,"New directory (size %d) has been built.",
           (int)the_directory->dir_len);
  log_debug(LD_DIRSERV,"New directory (size %d):\n%s",
            (int)the_directory->dir_len, the_directory->dir);

  the_directory_is_dirty = 0;

  /* Save the directory to disk so we re-load it quickly on startup.
   */
  dirserv_set_cached_directory(the_directory->dir, time(NULL), 0);

  return the_directory;
}