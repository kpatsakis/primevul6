ipcp_val_agg_replacement_ok_p (ipa_agg_replacement_value *aggvals,
			       int index, HOST_WIDE_INT offset, tree value)
{
  if (offset == -1)
    return true;

  while (aggvals)
    {
      if (aggvals->index == index
	  && aggvals->offset == offset
	  && values_equal_for_ipcp_p (aggvals->value, value))
	return true;
      aggvals = aggvals->next;
    }
  return false;
}