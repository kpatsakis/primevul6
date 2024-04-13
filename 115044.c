ipcp_cloning_candidate_p (struct cgraph_node *node)
{
  struct caller_statistics stats;

  gcc_checking_assert (node->has_gimple_body_p ());

  if (!opt_for_fn (node->decl, flag_ipa_cp_clone))
    {
      if (dump_file)
	fprintf (dump_file, "Not considering %s for cloning; "
		 "-fipa-cp-clone disabled.\n",
 		 node->name ());
      return false;
    }

  if (node->optimize_for_size_p ())
    {
      if (dump_file)
	fprintf (dump_file, "Not considering %s for cloning; "
		 "optimizing it for size.\n",
 		 node->name ());
      return false;
    }

  init_caller_stats (&stats);
  node->call_for_symbol_thunks_and_aliases (gather_caller_stats, &stats, false);

  if (ipa_size_summaries->get (node)->self_size < stats.n_calls)
    {
      if (dump_file)
	fprintf (dump_file, "Considering %s for cloning; code might shrink.\n",
 		 node->name ());
      return true;
    }

  /* When profile is available and function is hot, propagate into it even if
     calls seems cold; constant propagation can improve function's speed
     significantly.  */
  if (max_count > profile_count::zero ())
    {
      if (stats.count_sum > node->count.ipa ().apply_scale (90, 100))
	{
	  if (dump_file)
	    fprintf (dump_file, "Considering %s for cloning; "
		     "usually called directly.\n",
		     node->name ());
	  return true;
	}
    }
  if (!stats.n_hot_calls)
    {
      if (dump_file)
	fprintf (dump_file, "Not considering %s for cloning; no hot calls.\n",
		 node->name ());
      return false;
    }
  if (dump_file)
    fprintf (dump_file, "Considering %s for cloning.\n",
	     node->name ());
  return true;
}