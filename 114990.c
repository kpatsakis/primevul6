set_all_contains_variable (class ipcp_param_lattices *plats)
{
  bool ret;
  ret = plats->itself.set_contains_variable ();
  ret |= plats->ctxlat.set_contains_variable ();
  ret |= set_agg_lats_contain_variable (plats);
  ret |= plats->bits_lattice.set_to_bottom ();
  ret |= plats->m_value_range.set_to_bottom ();
  return ret;
}