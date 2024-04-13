gather_context_independent_values (class ipa_node_params *info,
				   vec<tree> *known_csts,
				   vec<ipa_polymorphic_call_context>
				   *known_contexts,
				   vec<ipa_agg_value_set> *known_aggs,
				   int *removable_params_cost)
{
  int i, count = ipa_get_param_count (info);
  bool ret = false;

  known_csts->create (0);
  known_contexts->create (0);
  known_csts->safe_grow_cleared (count);
  known_contexts->safe_grow_cleared (count);
  if (known_aggs)
    {
      known_aggs->create (0);
      known_aggs->safe_grow_cleared (count);
    }

  if (removable_params_cost)
    *removable_params_cost = 0;

  for (i = 0; i < count; i++)
    {
      class ipcp_param_lattices *plats = ipa_get_parm_lattices (info, i);
      ipcp_lattice<tree> *lat = &plats->itself;

      if (lat->is_single_const ())
	{
	  ipcp_value<tree> *val = lat->values;
	  gcc_checking_assert (TREE_CODE (val->value) != TREE_BINFO);
	  (*known_csts)[i] = val->value;
	  if (removable_params_cost)
	    *removable_params_cost
	      += estimate_move_cost (TREE_TYPE (val->value), false);
	  ret = true;
	}
      else if (removable_params_cost
	       && !ipa_is_param_used (info, i))
	*removable_params_cost
	  += ipa_get_param_move_cost (info, i);

      if (!ipa_is_param_used (info, i))
	continue;

      ipcp_lattice<ipa_polymorphic_call_context> *ctxlat = &plats->ctxlat;
      /* Do not account known context as reason for cloning.  We can see
	 if it permits devirtualization.  */
      if (ctxlat->is_single_const ())
	(*known_contexts)[i] = ctxlat->values->value;

      if (known_aggs)
	{
	  vec<ipa_agg_value> agg_items;
	  struct ipa_agg_value_set *agg;

	  agg_items = context_independent_aggregate_values (plats);
	  agg = &(*known_aggs)[i];
	  agg->items = agg_items;
	  agg->by_ref = plats->aggs_by_ref;
	  ret |= !agg_items.is_empty ();
	}
    }

  return ret;
}