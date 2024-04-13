set_agg_lats_contain_variable (class ipcp_param_lattices *plats)
{
  bool ret = !plats->aggs_contain_variable;
  plats->aggs_contain_variable = true;
  return ret;
}