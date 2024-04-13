update_profiling_info (struct cgraph_node *orig_node,
		       struct cgraph_node *new_node)
{
  struct cgraph_edge *cs;
  struct caller_statistics stats;
  profile_count new_sum, orig_sum;
  profile_count remainder, orig_node_count = orig_node->count;
  profile_count orig_new_node_count = new_node->count;

  if (!(orig_node_count.ipa () > profile_count::zero ()))
    return;

  init_caller_stats (&stats);
  orig_node->call_for_symbol_thunks_and_aliases (gather_caller_stats, &stats,
					       false);
  orig_sum = stats.count_sum;
  init_caller_stats (&stats);
  new_node->call_for_symbol_thunks_and_aliases (gather_caller_stats, &stats,
					      false);
  new_sum = stats.count_sum;

  if (orig_node_count < orig_sum + new_sum)
    {
      if (dump_file)
	{
	  fprintf (dump_file, "    Problem: node %s has too low count ",
		   orig_node->dump_name ());
	  orig_node_count.dump (dump_file);
	  fprintf (dump_file, "while the sum of incoming count is ");
	  (orig_sum + new_sum).dump (dump_file);
	  fprintf (dump_file, "\n");
	}

      orig_node_count = (orig_sum + new_sum).apply_scale (12, 10);
      if (dump_file)
	{
	  fprintf (dump_file, "      proceeding by pretending it was ");
	  orig_node_count.dump (dump_file);
	  fprintf (dump_file, "\n");
	}
    }

  remainder = orig_node_count.combine_with_ipa_count (orig_node_count.ipa ()
						      - new_sum.ipa ());

  /* With partial train run we do not want to assume that original's
     count is zero whenever we redurect all executed edges to clone.
     Simply drop profile to local one in this case.  */
  if (remainder.ipa_p () && !remainder.ipa ().nonzero_p ()
      && orig_node->count.ipa_p () && orig_node->count.ipa ().nonzero_p ()
      && flag_profile_partial_training)
    remainder = remainder.guessed_local ();

  new_sum = orig_node_count.combine_with_ipa_count (new_sum);
  new_node->count = new_sum;
  orig_node->count = remainder;

  profile_count::adjust_for_ipa_scaling (&new_sum, &orig_new_node_count);
  for (cs = new_node->callees; cs; cs = cs->next_callee)
    cs->count = cs->count.apply_scale (new_sum, orig_new_node_count);
  for (cs = new_node->indirect_calls; cs; cs = cs->next_callee)
    cs->count = cs->count.apply_scale (new_sum, orig_new_node_count);

  profile_count::adjust_for_ipa_scaling (&remainder, &orig_node_count);
  for (cs = orig_node->callees; cs; cs = cs->next_callee)
    cs->count = cs->count.apply_scale (remainder, orig_node_count);
  for (cs = orig_node->indirect_calls; cs; cs = cs->next_callee)
    cs->count = cs->count.apply_scale (remainder, orig_node_count);

  if (dump_file)
    dump_profile_updates (orig_node, new_node);
}