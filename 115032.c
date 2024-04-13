propagate_aggs_across_jump_function (struct cgraph_edge *cs,
				     struct ipa_jump_func *jfunc,
				     class ipcp_param_lattices *dest_plats)
{
  bool ret = false;

  if (dest_plats->aggs_bottom)
    return false;

  if (jfunc->type == IPA_JF_PASS_THROUGH
      && ipa_get_jf_pass_through_operation (jfunc) == NOP_EXPR)
    {
      class ipa_node_params *caller_info = IPA_NODE_REF (cs->caller);
      int src_idx = ipa_get_jf_pass_through_formal_id (jfunc);
      class ipcp_param_lattices *src_plats;

      src_plats = ipa_get_parm_lattices (caller_info, src_idx);
      if (agg_pass_through_permissible_p (src_plats, jfunc))
	{
	  /* Currently we do not produce clobber aggregate jump
	     functions, replace with merging when we do.  */
	  gcc_assert (!jfunc->agg.items);
	  ret |= merge_aggregate_lattices (cs, dest_plats, src_plats,
					   src_idx, 0);
	}
      else
	ret |= set_agg_lats_contain_variable (dest_plats);
    }
  else if (jfunc->type == IPA_JF_ANCESTOR
	   && ipa_get_jf_ancestor_agg_preserved (jfunc))
    {
      class ipa_node_params *caller_info = IPA_NODE_REF (cs->caller);
      int src_idx = ipa_get_jf_ancestor_formal_id (jfunc);
      class ipcp_param_lattices *src_plats;

      src_plats = ipa_get_parm_lattices (caller_info, src_idx);
      if (src_plats->aggs && src_plats->aggs_by_ref)
	{
	  /* Currently we do not produce clobber aggregate jump
	     functions, replace with merging when we do.  */
	  gcc_assert (!jfunc->agg.items);
	  ret |= merge_aggregate_lattices (cs, dest_plats, src_plats, src_idx,
					   ipa_get_jf_ancestor_offset (jfunc));
	}
      else if (!src_plats->aggs_by_ref)
	ret |= set_agg_lats_to_bottom (dest_plats);
      else
	ret |= set_agg_lats_contain_variable (dest_plats);
    }
  else if (jfunc->agg.items)
    {
      bool pre_existing = dest_plats->aggs != NULL;
      struct ipcp_agg_lattice **aglat = &dest_plats->aggs;
      struct ipa_agg_jf_item *item;
      int i;

      if (set_check_aggs_by_ref (dest_plats, jfunc->agg.by_ref))
	return true;

      FOR_EACH_VEC_ELT (*jfunc->agg.items, i, item)
	{
	  HOST_WIDE_INT val_size;

	  if (item->offset < 0 || item->jftype == IPA_JF_UNKNOWN)
	    continue;
	  val_size = tree_to_shwi (TYPE_SIZE (item->type));

	  if (merge_agg_lats_step (dest_plats, item->offset, val_size,
				   &aglat, pre_existing, &ret))
	    {
	      ret |= propagate_aggregate_lattice (cs, item, *aglat);
	      aglat = &(*aglat)->next;
	    }
	  else if (dest_plats->aggs_bottom)
	    return true;
	}

      ret |= set_chain_of_aglats_contains_variable (*aglat);
    }
  else
    ret |= set_agg_lats_contain_variable (dest_plats);

  return ret;
}