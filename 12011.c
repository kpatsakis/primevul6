should_generate_v2_networkstatus(void)
{
  return authdir_mode_v2(get_options()) &&
    the_v2_networkstatus_is_dirty &&
    the_v2_networkstatus_is_dirty + DIR_REGEN_SLACK_TIME < time(NULL);
}