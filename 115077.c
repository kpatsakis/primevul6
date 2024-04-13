copy_plats_to_inter (class ipcp_param_lattices *plats, HOST_WIDE_INT offset)
{
  vec<ipa_agg_value> res = vNULL;

  if (!plats->aggs || plats->aggs_contain_variable || plats->aggs_bottom)
    return vNULL;

  for (struct ipcp_agg_lattice *aglat = plats->aggs; aglat; aglat = aglat->next)
    if (aglat->is_single_const ())
      {
	struct ipa_agg_value ti;
	ti.offset = aglat->offset - offset;
	ti.value = aglat->values->value;
	res.safe_push (ti);
      }
  return res;
}