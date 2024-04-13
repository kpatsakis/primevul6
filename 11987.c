clear_status_flags_on_sybil(routerstatus_t *rs)
{
  rs->is_authority = rs->is_exit = rs->is_stable = rs->is_fast =
    rs->is_running = rs->is_named = rs->is_valid = rs->is_v2_dir =
    rs->is_hs_dir = rs->is_possible_guard = rs->is_bad_exit =
    rs->is_bad_directory = 0;
  /* FFFF we might want some mechanism to check later on if we
   * missed zeroing any flags: it's easy to add a new flag but
   * forget to add it to this clause. */
}