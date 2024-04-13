context_independent_aggregate_values (class ipcp_param_lattices *plats)
{
  vec<ipa_agg_value> res = vNULL;

  if (plats->aggs_bottom
      || plats->aggs_contain_variable
      || plats->aggs_count == 0)
    return vNULL;

  for (struct ipcp_agg_lattice *aglat = plats->aggs;
       aglat;
       aglat = aglat->next)
    if (aglat->is_single_const ())
      {
	struct ipa_agg_value item;
	item.offset = aglat->offset;
	item.value = aglat->values->value;
	res.safe_push (item);
      }
  return res;
}