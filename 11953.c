dirserv_get_runningrouters(void)
{
  return dirserv_pick_cached_dir_obj(
                         &cached_runningrouters, &the_runningrouters,
                         runningrouters_is_dirty,
                         generate_runningrouters,
                         "v1 network status list", V1_AUTHORITY);
}