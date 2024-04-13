ipa_agg_value_set_from_jfunc (class ipa_node_params *info, cgraph_node *node,
			      struct ipa_agg_jump_function *agg_jfunc)
{
  struct ipa_agg_value_set agg;
  struct ipa_agg_jf_item *item;
  int i;

  agg.items = vNULL;
  agg.by_ref = agg_jfunc->by_ref;

  FOR_EACH_VEC_SAFE_ELT (agg_jfunc->items, i, item)
    {
      tree value = ipa_agg_value_from_node (info, node, item);

      if (value)
	{
	  struct ipa_agg_value value_item;

	  value_item.offset = item->offset;
	  value_item.value = value;

	  agg.items.safe_push (value_item);
	}
    }
  return agg;
}