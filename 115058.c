set_check_aggs_by_ref (class ipcp_param_lattices *dest_plats,
		       bool new_aggs_by_ref)
{
  if (dest_plats->aggs)
    {
      if (dest_plats->aggs_by_ref != new_aggs_by_ref)
	{
	  set_agg_lats_to_bottom (dest_plats);
	  return true;
	}
    }
  else
    dest_plats->aggs_by_ref = new_aggs_by_ref;
  return false;
}