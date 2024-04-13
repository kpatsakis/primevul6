running_long_enough_to_decide_unreachable(void)
{
  return time_of_process_start
    + get_options()->TestingAuthDirTimeToLearnReachability < approx_time();
}