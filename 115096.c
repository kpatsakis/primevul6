cgraph_edge_brings_value_p (cgraph_edge *cs, ipcp_value_source<tree> *src,
			    cgraph_node *dest, ipcp_value<tree> *dest_val)
{
  class ipa_node_params *caller_info = IPA_NODE_REF (cs->caller);
  enum availability availability;
  cgraph_node *real_dest = cs->callee->function_symbol (&availability);

  if (availability <= AVAIL_INTERPOSABLE
      || !same_node_or_its_all_contexts_clone_p (real_dest, dest)
      || caller_info->node_dead)
    return false;

  if (!src->val)
    return true;

  if (caller_info->ipcp_orig_node)
    {
      tree t;
      if (src->offset == -1)
	t = caller_info->known_csts[src->index];
      else
	t = get_clone_agg_value (cs->caller, src->offset, src->index);
      return (t != NULL_TREE
	      && values_equal_for_ipcp_p (src->val->value, t));
    }
  else
    {
      /* At the moment we do not propagate over arithmetic jump functions in
	 SCCs, so it is safe to detect self-feeding recursive calls in this
	 way.  */
      if (src->val == dest_val)
	return true;

      struct ipcp_agg_lattice *aglat;
      class ipcp_param_lattices *plats = ipa_get_parm_lattices (caller_info,
								 src->index);
      if (src->offset == -1)
	return (plats->itself.is_single_const ()
		&& values_equal_for_ipcp_p (src->val->value,
					    plats->itself.values->value));
      else
	{
	  if (plats->aggs_bottom || plats->aggs_contain_variable)
	    return false;
	  for (aglat = plats->aggs; aglat; aglat = aglat->next)
	    if (aglat->offset == src->offset)
	      return  (aglat->is_single_const ()
		       && values_equal_for_ipcp_p (src->val->value,
						   aglat->values->value));
	}
      return false;
    }
}