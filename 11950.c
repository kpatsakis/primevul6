directory_set_dirty(void)
{
  time_t now = time(NULL);
  int set_v1_dirty=0;

  /* Regenerate stubs only every 8 hours.
   * XXXX It would be nice to generate less often, but these are just
   * stubs: it doesn't matter. */
#define STUB_REGENERATE_INTERVAL (8*60*60)
  if (!the_directory || !the_runningrouters.dir)
    set_v1_dirty = 1;
  else if (the_directory->published < now - STUB_REGENERATE_INTERVAL ||
           the_runningrouters.published < now - STUB_REGENERATE_INTERVAL)
    set_v1_dirty = 1;

  if (set_v1_dirty) {
    if (!the_directory_is_dirty)
      the_directory_is_dirty = now;
    if (!runningrouters_is_dirty)
      runningrouters_is_dirty = now;
  }
  if (!the_v2_networkstatus_is_dirty)
    the_v2_networkstatus_is_dirty = now;
}