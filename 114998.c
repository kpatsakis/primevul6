perhaps_add_new_callers (cgraph_node *node, ipcp_value<valtype> *val)
{
  ipcp_value_source<valtype> *src;
  profile_count redirected_sum = profile_count::zero ();

  for (src = val->sources; src; src = src->next)
    {
      struct cgraph_edge *cs = src->cs;
      while (cs)
	{
	  if (cgraph_edge_brings_value_p (cs, src, node, val)
	      && cgraph_edge_brings_all_scalars_for_node (cs, val->spec_node)
	      && cgraph_edge_brings_all_agg_vals_for_node (cs, val->spec_node))
	    {
	      if (dump_file)
		fprintf (dump_file, " - adding an extra caller %s of %s\n",
			 cs->caller->dump_name (),
			 val->spec_node->dump_name ());

	      cs->redirect_callee_duplicating_thunks (val->spec_node);
	      val->spec_node->expand_all_artificial_thunks ();
	      if (cs->count.ipa ().initialized_p ())
	        redirected_sum = redirected_sum + cs->count.ipa ();
	    }
	  cs = get_next_cgraph_edge_clone (cs);
	}
    }

  if (redirected_sum.nonzero_p ())
    update_specialized_profile (val->spec_node, node, redirected_sum);
}