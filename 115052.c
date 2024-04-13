cgraph_edge_brings_all_agg_vals_for_node (struct cgraph_edge *cs,
					  struct cgraph_node *node)
{
  class ipa_node_params *orig_node_info;
  struct ipa_agg_replacement_value *aggval;
  int i, ec, count;

  aggval = ipa_get_agg_replacements_for_node (node);
  if (!aggval)
    return true;

  count = ipa_get_param_count (IPA_NODE_REF (node));
  ec = ipa_get_cs_argument_count (IPA_EDGE_REF (cs));
  if (ec < count)
    for (struct ipa_agg_replacement_value *av = aggval; av; av = av->next)
      if (aggval->index >= ec)
	return false;

  orig_node_info = IPA_NODE_REF (IPA_NODE_REF (node)->ipcp_orig_node);

  for (i = 0; i < count; i++)
    {
      class ipcp_param_lattices *plats;
      bool interesting = false;
      for (struct ipa_agg_replacement_value *av = aggval; av; av = av->next)
	if (aggval->index == i)
	  {
	    interesting = true;
	    break;
	  }
      if (!interesting)
	continue;

      plats = ipa_get_parm_lattices (orig_node_info, aggval->index);
      if (plats->aggs_bottom)
	return false;

      vec<ipa_agg_value> values = intersect_aggregates_with_edge (cs, i, vNULL);
      if (!values.exists ())
	return false;

      for (struct ipa_agg_replacement_value *av = aggval; av; av = av->next)
	if (aggval->index == i)
	  {
	    struct ipa_agg_value *item;
	    int j;
	    bool found = false;
	    FOR_EACH_VEC_ELT (values, j, item)
	      if (item->value
		  && item->offset == av->offset
		  && values_equal_for_ipcp_p (item->value, av->value))
		{
		  found = true;
		  break;
		}
	    if (!found)
	      {
		values.release ();
		return false;
	      }
	  }
      values.release ();
    }
  return true;
}