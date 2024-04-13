dirserv_free_all(void)
{
  dirserv_free_fingerprint_list();

  cached_dir_decref(the_directory);
  clear_cached_dir(&the_runningrouters);
  cached_dir_decref(the_v2_networkstatus);
  cached_dir_decref(cached_directory);
  clear_cached_dir(&cached_runningrouters);

  digestmap_free(cached_v2_networkstatus, _free_cached_dir);
  cached_v2_networkstatus = NULL;
  strmap_free(cached_consensuses, _free_cached_dir);
  cached_consensuses = NULL;
}