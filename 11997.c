dirserv_set_cached_directory(const char *directory, time_t published,
                             int is_running_routers)
{
  time_t now = time(NULL);

  if (is_running_routers) {
    if (published >= now - MAX_V1_RR_AGE)
      set_cached_dir(&cached_runningrouters, tor_strdup(directory), published);
  } else {
    if (published >= now - MAX_V1_DIRECTORY_AGE) {
      cached_dir_decref(cached_directory);
      cached_directory = new_cached_dir(tor_strdup(directory), published);
    }
  }
}