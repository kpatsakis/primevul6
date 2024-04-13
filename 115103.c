find_aggregate_values_for_callers_subset (struct cgraph_node *node,
					  vec<cgraph_edge *> callers)
{
  class ipa_node_params *dest_info = IPA_NODE_REF (node);
  struct ipa_agg_replacement_value *res;
  struct ipa_agg_replacement_value **tail = &res;
  struct cgraph_edge *cs;
  int i, j, count = ipa_get_param_count (dest_info);

  FOR_EACH_VEC_ELT (callers, j, cs)
    {
      if (!IPA_EDGE_REF (cs))
	{
	  count = 0;
	  break;
	}
      int c = ipa_get_cs_argument_count (IPA_EDGE_REF (cs));
      if (c < count)
	count = c;
    }

  for (i = 0; i < count; i++)
    {
      struct cgraph_edge *cs;
      vec<ipa_agg_value> inter = vNULL;
      struct ipa_agg_value *item;
      class ipcp_param_lattices *plats = ipa_get_parm_lattices (dest_info, i);
      int j;

      /* Among other things, the following check should deal with all by_ref
	 mismatches.  */
      if (plats->aggs_bottom)
	continue;

      FOR_EACH_VEC_ELT (callers, j, cs)
	{
	  struct ipa_jump_func *jfunc
	    = ipa_get_ith_jump_func (IPA_EDGE_REF (cs), i);
	  if (self_recursive_pass_through_p (cs, jfunc, i)
	      && (!plats->aggs_by_ref
		  || ipa_get_jf_pass_through_agg_preserved (jfunc)))
	    continue;
	  inter = intersect_aggregates_with_edge (cs, i, inter);

	  if (!inter.exists ())
	    goto next_param;
	}

      FOR_EACH_VEC_ELT (inter, j, item)
	{
	  struct ipa_agg_replacement_value *v;

	  if (!item->value)
	    continue;

	  /* All values must be real values, not unknown place holders.  */
	  gcc_assert (item->value != error_mark_node);

	  v = ggc_alloc<ipa_agg_replacement_value> ();
	  v->index = i;
	  v->offset = item->offset;
	  v->value = item->value;
	  v->by_ref = plats->aggs_by_ref;
	  *tail = v;
	  tail = &v->next;
	}

    next_param:
      if (inter.exists ())
	inter.release ();
    }
  *tail = NULL;
  return res;
}