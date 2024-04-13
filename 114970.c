identify_dead_nodes (struct cgraph_node *node)
{
  struct cgraph_node *v;
  for (v = node; v; v = ((struct ipa_dfs_info *) v->aux)->next_cycle)
    if (v->local
	&& IPA_NODE_REF (v)
	&& !v->call_for_symbol_thunks_and_aliases
	     (has_undead_caller_from_outside_scc_p, NULL, true))
      IPA_NODE_REF (v)->node_dead = 1;

  for (v = node; v; v = ((struct ipa_dfs_info *) v->aux)->next_cycle)
    if (IPA_NODE_REF (v) && !IPA_NODE_REF (v)->node_dead)
      spread_undeadness (v);

  if (dump_file && (dump_flags & TDF_DETAILS))
    {
      for (v = node; v; v = ((struct ipa_dfs_info *) v->aux)->next_cycle)
	if (IPA_NODE_REF (v) && IPA_NODE_REF (v)->node_dead)
	  fprintf (dump_file, "  Marking node as dead: %s.\n", v->dump_name ());
    }
}