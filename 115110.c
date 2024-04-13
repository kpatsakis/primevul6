init_caller_stats (struct caller_statistics *stats)
{
  stats->count_sum = profile_count::zero ();
  stats->n_calls = 0;
  stats->n_hot_calls = 0;
  stats->freq_sum = 0;
}