initialize_node_lattices (struct cgraph_node *node)
{
  class ipa_node_params *info = IPA_NODE_REF (node);
  struct cgraph_edge *ie;
  bool disable = false, variable = false;
  int i;

  gcc_checking_assert (node->has_gimple_body_p ());

  if (!ipa_get_param_count (info))
    disable = true;
  else if (node->local)
    {
      int caller_count = 0;
      node->call_for_symbol_thunks_and_aliases (count_callers, &caller_count,
						true);
      gcc_checking_assert (caller_count > 0);
      if (caller_count == 1)
	node->call_for_symbol_thunks_and_aliases (set_single_call_flag,
						  NULL, true);
    }
  else
    {
      /* When cloning is allowed, we can assume that externally visible
	 functions are not called.  We will compensate this by cloning
	 later.  */
      if (ipcp_versionable_function_p (node)
	  && ipcp_cloning_candidate_p (node))
	variable = true;
      else
	disable = true;
    }

  if (dump_file && (dump_flags & TDF_DETAILS)
      && !node->alias && !node->thunk.thunk_p)
    {
      fprintf (dump_file, "Initializing lattices of %s\n",
	       node->dump_name ());
      if (disable || variable)
	fprintf (dump_file, "  Marking all lattices as %s\n",
		 disable ? "BOTTOM" : "VARIABLE");
    }

  auto_vec<bool, 16> surviving_params;
  bool pre_modified = false;
  if (!disable && node->clone.param_adjustments)
    {
      /* At the moment all IPA optimizations should use the number of
	 parameters of the prevailing decl as the m_always_copy_start.
	 Handling any other value would complicate the code below, so for the
	 time bing let's only assert it is so.  */
      gcc_assert ((node->clone.param_adjustments->m_always_copy_start
		   == ipa_get_param_count (info))
		  || node->clone.param_adjustments->m_always_copy_start < 0);

      pre_modified = true;
      node->clone.param_adjustments->get_surviving_params (&surviving_params);

      if (dump_file && (dump_flags & TDF_DETAILS)
	  && !node->alias && !node->thunk.thunk_p)
	{
	  bool first = true;
	  for (int j = 0; j < ipa_get_param_count (info); j++)
	    {
	      if (j < (int) surviving_params.length ()
		  && surviving_params[j])
		continue;
	      if (first)
		{
		  fprintf (dump_file,
			   "  The following parameters are dead on arrival:");
		  first = false;
		}
	      fprintf (dump_file, " %u", j);
	    }
	  if (!first)
	      fprintf (dump_file, "\n");
	}
    }

  for (i = 0; i < ipa_get_param_count (info); i++)
    {
      ipcp_param_lattices *plats = ipa_get_parm_lattices (info, i);
      if (disable
	  || (pre_modified && (surviving_params.length () <= (unsigned) i
			       || !surviving_params[i])))
	{
	  plats->itself.set_to_bottom ();
	  plats->ctxlat.set_to_bottom ();
	  set_agg_lats_to_bottom (plats);
	  plats->bits_lattice.set_to_bottom ();
	  plats->m_value_range.set_to_bottom ();
	}
      else
	{
	  plats->m_value_range.init ();
	  if (variable)
	    set_all_contains_variable (plats);
	}
    }

  for (ie = node->indirect_calls; ie; ie = ie->next_callee)
    if (ie->indirect_info->polymorphic
	&& ie->indirect_info->param_index >= 0)
      {
	gcc_checking_assert (ie->indirect_info->param_index >= 0);
	ipa_get_parm_lattices (info,
			       ie->indirect_info->param_index)->virt_call = 1;
      }
}