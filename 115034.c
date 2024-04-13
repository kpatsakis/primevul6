propagate_aggregate_lattice (struct cgraph_edge *cs,
			     struct ipa_agg_jf_item *item,
			     struct ipcp_agg_lattice *aglat)
{
  class ipa_node_params *caller_info;
  class ipcp_param_lattices *src_plats;
  struct ipcp_lattice<tree> *src_lat;
  HOST_WIDE_INT src_offset;
  int src_idx;
  tree load_type;
  bool ret;

  if (item->jftype == IPA_JF_CONST)
    {
      tree value = item->value.constant;

      gcc_checking_assert (is_gimple_ip_invariant (value));
      return aglat->add_value (value, cs, NULL, 0);
    }

  gcc_checking_assert (item->jftype == IPA_JF_PASS_THROUGH
		       || item->jftype == IPA_JF_LOAD_AGG);

  caller_info = IPA_NODE_REF (cs->caller);
  src_idx = item->value.pass_through.formal_id;
  src_plats = ipa_get_parm_lattices (caller_info, src_idx);

  if (item->jftype == IPA_JF_PASS_THROUGH)
    {
      load_type = NULL_TREE;
      src_lat = &src_plats->itself;
      src_offset = -1;
    }
  else
    {
      HOST_WIDE_INT load_offset = item->value.load_agg.offset;
      struct ipcp_agg_lattice *src_aglat;

      for (src_aglat = src_plats->aggs; src_aglat; src_aglat = src_aglat->next)
	if (src_aglat->offset >= load_offset)
	  break;

      load_type = item->value.load_agg.type;
      if (!src_aglat
	  || src_aglat->offset > load_offset
	  || src_aglat->size != tree_to_shwi (TYPE_SIZE (load_type))
	  || src_plats->aggs_by_ref != item->value.load_agg.by_ref)
	return aglat->set_contains_variable ();

      src_lat = src_aglat;
      src_offset = load_offset;
    }

  if (src_lat->bottom
      || (!ipcp_versionable_function_p (cs->caller)
	  && !src_lat->is_single_const ()))
    return aglat->set_contains_variable ();

  ret = propagate_vals_across_arith_jfunc (cs,
					   item->value.pass_through.operation,
					   load_type,
					   item->value.pass_through.operand,
					   src_lat, aglat,
					   src_offset,
					   src_idx,
					   item->type);

  if (src_lat->contains_variable)
    ret |= aglat->set_contains_variable ();

  return ret;
}