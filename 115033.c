add_all_node_vals_to_toposort (cgraph_node *node, ipa_topo_info *topo)
{
  class ipa_node_params *info = IPA_NODE_REF (node);
  int i, count = ipa_get_param_count (info);

  for (i = 0; i < count; i++)
    {
      class ipcp_param_lattices *plats = ipa_get_parm_lattices (info, i);
      ipcp_lattice<tree> *lat = &plats->itself;
      struct ipcp_agg_lattice *aglat;

      if (!lat->bottom)
	{
	  ipcp_value<tree> *val;
	  for (val = lat->values; val; val = val->next)
	    topo->constants.add_val (val);
	}

      if (!plats->aggs_bottom)
	for (aglat = plats->aggs; aglat; aglat = aglat->next)
	  if (!aglat->bottom)
	    {
	      ipcp_value<tree> *val;
	      for (val = aglat->values; val; val = val->next)
		topo->constants.add_val (val);
	    }

      ipcp_lattice<ipa_polymorphic_call_context> *ctxlat = &plats->ctxlat;
      if (!ctxlat->bottom)
	{
	  ipcp_value<ipa_polymorphic_call_context> *ctxval;
	  for (ctxval = ctxlat->values; ctxval; ctxval = ctxval->next)
	    topo->contexts.add_val (ctxval);
	}
    }
}