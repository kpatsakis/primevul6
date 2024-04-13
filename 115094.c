has_undead_caller_from_outside_scc_p (struct cgraph_node *node,
				      void *data ATTRIBUTE_UNUSED)
{
  struct cgraph_edge *cs;

  for (cs = node->callers; cs; cs = cs->next_caller)
    if (cs->caller->thunk.thunk_p
	&& cs->caller->call_for_symbol_thunks_and_aliases
	  (has_undead_caller_from_outside_scc_p, NULL, true))
      return true;
    else if (!ipa_edge_within_scc (cs)
	     && !IPA_NODE_REF (cs->caller)->node_dead)
      return true;
  return false;
}