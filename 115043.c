intersect_with_plats (class ipcp_param_lattices *plats,
		      vec<ipa_agg_value> *inter,
		      HOST_WIDE_INT offset)
{
  struct ipcp_agg_lattice *aglat;
  struct ipa_agg_value *item;
  int k;

  if (!plats->aggs || plats->aggs_contain_variable || plats->aggs_bottom)
    {
      inter->release ();
      return;
    }

  aglat = plats->aggs;
  FOR_EACH_VEC_ELT (*inter, k, item)
    {
      bool found = false;
      if (!item->value)
	continue;
      while (aglat)
	{
	  if (aglat->offset - offset > item->offset)
	    break;
	  if (aglat->offset - offset == item->offset)
	    {
	      gcc_checking_assert (item->value);
	      if (aglat->is_single_const ())
		{
		  tree value = aglat->values->value;

		  if (values_equal_for_ipcp_p (item->value, value))
		    found = true;
		  else if (item->value == error_mark_node)
		    {
		      /* Replace unknown place holder value with real one.  */
		      item->value = value;
		      found = true;
		    }
		}
	      break;
	    }
	  aglat = aglat->next;
	}
      if (!found)
	item->value = NULL_TREE;
    }
}