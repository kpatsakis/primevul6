intersect_with_agg_replacements (struct cgraph_node *node, int index,
				 vec<ipa_agg_value> *inter,
				 HOST_WIDE_INT offset)
{
  struct ipa_agg_replacement_value *srcvals;
  struct ipa_agg_value *item;
  int i;

  srcvals = ipa_get_agg_replacements_for_node (node);
  if (!srcvals)
    {
      inter->release ();
      return;
    }

  FOR_EACH_VEC_ELT (*inter, i, item)
    {
      struct ipa_agg_replacement_value *av;
      bool found = false;
      if (!item->value)
	continue;
      for (av = srcvals; av; av = av->next)
	{
	  gcc_checking_assert (av->value);
	  if (av->index == index
	      && av->offset - offset == item->offset)
	    {
	      if (values_equal_for_ipcp_p (item->value, av->value))
		found = true;
	      else if (item->value == error_mark_node)
		{
		  /* Replace place holder value with real one.  */
		  item->value = av->value;
		  found = true;
		}
	      break;
	    }
	}
      if (!found)
	item->value = NULL_TREE;
    }
}