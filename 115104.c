spread_undeadness (struct cgraph_node *node)
{
  struct cgraph_edge *cs;

  for (cs = node->callees; cs; cs = cs->next_callee)
    if (ipa_edge_within_scc (cs))
      {
	struct cgraph_node *callee;
	class ipa_node_params *info;

	callee = cs->callee->function_symbol (NULL);
	info = IPA_NODE_REF (callee);

	if (info && info->node_dead)
	  {
	    info->node_dead = 0;
	    spread_undeadness (callee);
	  }
      }
}