get_clone_agg_value (struct cgraph_node *node, HOST_WIDE_INT offset,
		     int index)
{
  struct ipa_agg_replacement_value *aggval;

  aggval = ipa_get_agg_replacements_for_node (node);
  while (aggval)
    {
      if (aggval->offset == offset
	  && aggval->index == index)
	return aggval->value;
      aggval = aggval->next;
    }
  return NULL_TREE;
}