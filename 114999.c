ipa_agg_value_from_node (class ipa_node_params *info,
			 struct cgraph_node *node,
			 struct ipa_agg_jf_item *item)
{
  tree value = NULL_TREE;
  int src_idx;

  if (item->offset < 0 || item->jftype == IPA_JF_UNKNOWN)
    return NULL_TREE;

  if (item->jftype == IPA_JF_CONST)
    return item->value.constant;

  gcc_checking_assert (item->jftype == IPA_JF_PASS_THROUGH
		       || item->jftype == IPA_JF_LOAD_AGG);

  src_idx = item->value.pass_through.formal_id;

  if (info->ipcp_orig_node)
    {
      if (item->jftype == IPA_JF_PASS_THROUGH)
	value = info->known_csts[src_idx];
      else
	value = get_clone_agg_value (node, item->value.load_agg.offset,
				     src_idx);
    }
  else if (info->lattices)
    {
      class ipcp_param_lattices *src_plats
	= ipa_get_parm_lattices (info, src_idx);

      if (item->jftype == IPA_JF_PASS_THROUGH)
	{
	  struct ipcp_lattice<tree> *lat = &src_plats->itself;

	  if (!lat->is_single_const ())
	    return NULL_TREE;

	  value = lat->values->value;
	}
      else if (src_plats->aggs
	       && !src_plats->aggs_bottom
	       && !src_plats->aggs_contain_variable
	       && src_plats->aggs_by_ref == item->value.load_agg.by_ref)
	{
	  struct ipcp_agg_lattice *aglat;

	  for (aglat = src_plats->aggs; aglat; aglat = aglat->next)
	    {
	      if (aglat->offset > item->value.load_agg.offset)
		break;

	      if (aglat->offset == item->value.load_agg.offset)
		{
		  if (aglat->is_single_const ())
		    value = aglat->values->value;
		  break;
		}
	    }
	}
    }

  if (!value)
    return NULL_TREE;

  if (item->jftype == IPA_JF_LOAD_AGG)
    {
      tree load_type = item->value.load_agg.type;
      tree value_type = TREE_TYPE (value);

      /* Ensure value type is compatible with load type.  */
      if (!useless_type_conversion_p (load_type, value_type))
	return NULL_TREE;
    }

  return ipa_get_jf_arith_result (item->value.pass_through.operation,
				  value,
				  item->value.pass_through.operand,
				  item->type);
}