self_recursively_generated_p (ipcp_value<tree> *val)
{
  class ipa_node_params *info = NULL;

  for (ipcp_value_source<tree> *src = val->sources; src; src = src->next)
    {
      cgraph_edge *cs = src->cs;

      if (!src->val || cs->caller != cs->callee->function_symbol ()
	  || src->val == val)
	return false;

      if (!info)
	info = IPA_NODE_REF (cs->caller);

      class ipcp_param_lattices *plats = ipa_get_parm_lattices (info,
								src->index);
      ipcp_lattice<tree> *src_lat = src->offset == -1 ? &plats->itself
						      : plats->aggs;
      ipcp_value<tree> *src_val;

      for (src_val = src_lat->values; src_val; src_val = src_val->next)
	if (src_val == val)
	  break;

      if (!src_val)
	return false;
    }

  return true;
}