ipcp_decision_stage (class ipa_topo_info *topo)
{
  int i;

  if (dump_file)
    fprintf (dump_file, "\nIPA decision stage:\n\n");

  for (i = topo->nnodes - 1; i >= 0; i--)
    {
      struct cgraph_node *node = topo->order[i];
      bool change = false, iterate = true;

      while (iterate)
	{
	  struct cgraph_node *v;
	  iterate = false;
	  for (v = node; v; v = ((struct ipa_dfs_info *) v->aux)->next_cycle)
	    if (v->has_gimple_body_p ()
		&& ipcp_versionable_function_p (v))
	      iterate |= decide_whether_version_node (v);

	  change |= iterate;
	}
      if (change)
	identify_dead_nodes (node);
    }
}