gather_caller_stats (struct cgraph_node *node, void *data)
{
  struct caller_statistics *stats = (struct caller_statistics *) data;
  struct cgraph_edge *cs;

  for (cs = node->callers; cs; cs = cs->next_caller)
    if (!cs->caller->thunk.thunk_p)
      {
        if (cs->count.ipa ().initialized_p ())
	  stats->count_sum += cs->count.ipa ();
	stats->freq_sum += cs->frequency ();
	stats->n_calls++;
	if (cs->maybe_hot_p ())
	  stats->n_hot_calls ++;
      }
  return false;

}