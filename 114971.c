merge_agg_lats_step (class ipcp_param_lattices *dest_plats,
		     HOST_WIDE_INT offset, HOST_WIDE_INT val_size,
		     struct ipcp_agg_lattice ***aglat,
		     bool pre_existing, bool *change)
{
  gcc_checking_assert (offset >= 0);

  while (**aglat && (**aglat)->offset < offset)
    {
      if ((**aglat)->offset + (**aglat)->size > offset)
	{
	  set_agg_lats_to_bottom (dest_plats);
	  return false;
	}
      *change |= (**aglat)->set_contains_variable ();
      *aglat = &(**aglat)->next;
    }

  if (**aglat && (**aglat)->offset == offset)
    {
      if ((**aglat)->size != val_size)
	{
	  set_agg_lats_to_bottom (dest_plats);
	  return false;
	}
      gcc_assert (!(**aglat)->next
		  || (**aglat)->next->offset >= offset + val_size);
      return true;
    }
  else
    {
      struct ipcp_agg_lattice *new_al;

      if (**aglat && (**aglat)->offset < offset + val_size)
	{
	  set_agg_lats_to_bottom (dest_plats);
	  return false;
	}
      if (dest_plats->aggs_count == param_ipa_max_agg_items)
	return false;
      dest_plats->aggs_count++;
      new_al = ipcp_agg_lattice_pool.allocate ();
      memset (new_al, 0, sizeof (*new_al));

      new_al->offset = offset;
      new_al->size = val_size;
      new_al->contains_variable = pre_existing;

      new_al->next = **aglat;
      **aglat = new_al;
      return true;
    }
}