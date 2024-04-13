decide_whether_version_node (struct cgraph_node *node)
{
  class ipa_node_params *info = IPA_NODE_REF (node);
  int i, count = ipa_get_param_count (info);
  vec<tree> known_csts;
  vec<ipa_polymorphic_call_context> known_contexts;
  bool ret = false;

  if (count == 0)
    return false;

  if (dump_file && (dump_flags & TDF_DETAILS))
    fprintf (dump_file, "\nEvaluating opportunities for %s.\n",
	     node->dump_name ());

  gather_context_independent_values (info, &known_csts, &known_contexts,
				     NULL, NULL);

  for (i = 0; i < count;i++)
    {
      class ipcp_param_lattices *plats = ipa_get_parm_lattices (info, i);
      ipcp_lattice<tree> *lat = &plats->itself;
      ipcp_lattice<ipa_polymorphic_call_context> *ctxlat = &plats->ctxlat;

      if (!lat->bottom
	  && !known_csts[i])
	{
	  ipcp_value<tree> *val;
	  for (val = lat->values; val; val = val->next)
	    ret |= decide_about_value (node, i, -1, val, known_csts,
				       known_contexts);
	}

      if (!plats->aggs_bottom)
	{
	  struct ipcp_agg_lattice *aglat;
	  ipcp_value<tree> *val;
	  for (aglat = plats->aggs; aglat; aglat = aglat->next)
	    if (!aglat->bottom && aglat->values
		/* If the following is false, the one value is in
		   known_aggs.  */
		&& (plats->aggs_contain_variable
		    || !aglat->is_single_const ()))
	      for (val = aglat->values; val; val = val->next)
		ret |= decide_about_value (node, i, aglat->offset, val,
					   known_csts, known_contexts);
	}

      if (!ctxlat->bottom
	  && known_contexts[i].useless_p ())
	{
	  ipcp_value<ipa_polymorphic_call_context> *val;
	  for (val = ctxlat->values; val; val = val->next)
	    ret |= decide_about_value (node, i, -1, val, known_csts,
				       known_contexts);
	}

	info = IPA_NODE_REF (node);
    }

  if (info->do_clone_for_all_contexts)
    {
      struct cgraph_node *clone;
      vec<cgraph_edge *> callers;

      if (dump_file)
	fprintf (dump_file, " - Creating a specialized node of %s "
		 "for all known contexts.\n", node->dump_name ());

      callers = node->collect_callers ();
      find_more_scalar_values_for_callers_subset (node, known_csts, callers);
      find_more_contexts_for_caller_subset (node, &known_contexts, callers);
      ipa_agg_replacement_value *aggvals
	= find_aggregate_values_for_callers_subset (node, callers);

      if (!known_contexts_useful_p (known_contexts))
	{
	  known_contexts.release ();
	  known_contexts = vNULL;
	}
      clone = create_specialized_node (node, known_csts, known_contexts,
				       aggvals, callers);
      info = IPA_NODE_REF (node);
      info->do_clone_for_all_contexts = false;
      IPA_NODE_REF (clone)->is_all_contexts_clone = true;
      ret = true;
    }
  else
    {
      known_csts.release ();
      known_contexts.release ();
    }

  return ret;
}