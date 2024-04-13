set_agg_lats_to_bottom (class ipcp_param_lattices *plats)
{
  bool ret = !plats->aggs_bottom;
  plats->aggs_bottom = true;
  return ret;
}