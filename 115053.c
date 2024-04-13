agg_replacements_to_vector (struct cgraph_node *node, int index,
			    HOST_WIDE_INT offset)
{
  struct ipa_agg_replacement_value *av;
  vec<ipa_agg_value> res = vNULL;

  for (av = ipa_get_agg_replacements_for_node (node); av; av = av->next)
    if (av->index == index
	&& (av->offset - offset) >= 0)
    {
      struct ipa_agg_value item;
      gcc_checking_assert (av->value);
      item.offset = av->offset - offset;
      item.value = av->value;
      res.safe_push (item);
    }

  return res;
}