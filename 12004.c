dirserv_clear_old_v1_info(time_t now)
{
  if (cached_directory &&
      cached_directory->published < (now - MAX_V1_DIRECTORY_AGE)) {
    cached_dir_decref(cached_directory);
    cached_directory = NULL;
  }
  if (cached_runningrouters.published < (now - MAX_V1_RR_AGE)) {
    clear_cached_dir(&cached_runningrouters);
  }
}