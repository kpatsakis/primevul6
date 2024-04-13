merge_aggregate_lattices (struct cgraph_edge *cs,
			  class ipcp_param_lattices *dest_plats,
			  class ipcp_param_lattices *src_plats,
			  int src_idx, HOST_WIDE_INT offset_delta)
{
  bool pre_existing = dest_plats->aggs != NULL;
  struct ipcp_agg_lattice **dst_aglat;
  bool ret = false;

  if (set_check_aggs_by_ref (dest_plats, src_plats->aggs_by_ref))
    return true;
  if (src_plats->aggs_bottom)
    return set_agg_lats_contain_variable (dest_plats);
  if (src_plats->aggs_contain_variable)
    ret |= set_agg_lats_contain_variable (dest_plats);
  dst_aglat = &dest_plats->aggs;

  for (struct ipcp_agg_lattice *src_aglat = src_plats->aggs;
       src_aglat;
       src_aglat = src_aglat->next)
    {
      HOST_WIDE_INT new_offset = src_aglat->offset - offset_delta;

      if (new_offset < 0)
	continue;
      if (merge_agg_lats_step (dest_plats, new_offset, src_aglat->size,
			       &dst_aglat, pre_existing, &ret))
	{
	  struct ipcp_agg_lattice *new_al = *dst_aglat;

	  dst_aglat = &(*dst_aglat)->next;
	  if (src_aglat->bottom)
	    {
	      ret |= new_al->set_contains_variable ();
	      continue;
	    }
	  if (src_aglat->contains_variable)
	    ret |= new_al->set_contains_variable ();
	  for (ipcp_value<tree> *val = src_aglat->values;
	       val;
	       val = val->next)
	    ret |= new_al->add_value (val->value, cs, val, src_idx,
				      src_aglat->offset);
	}
      else if (dest_plats->aggs_bottom)
	return true;
    }
  ret |= set_chain_of_aglats_contains_variable (*dst_aglat);
  return ret;
}