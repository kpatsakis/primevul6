find_more_contexts_for_caller_subset (cgraph_node *node,
				      vec<ipa_polymorphic_call_context>
				      *known_contexts,
				      vec<cgraph_edge *> callers)
{
  ipa_node_params *info = IPA_NODE_REF (node);
  int i, count = ipa_get_param_count (info);

  for (i = 0; i < count; i++)
    {
      cgraph_edge *cs;

      if (ipa_get_poly_ctx_lat (info, i)->bottom
	  || (known_contexts->exists ()
	      && !(*known_contexts)[i].useless_p ()))
	continue;

      ipa_polymorphic_call_context newval;
      bool first = true;
      int j;

      FOR_EACH_VEC_ELT (callers, j, cs)
	{
	  if (!IPA_EDGE_REF (cs)
	      || i >= ipa_get_cs_argument_count (IPA_EDGE_REF (cs)))
	    return;
	  ipa_jump_func *jfunc = ipa_get_ith_jump_func (IPA_EDGE_REF (cs),
							    i);
	  ipa_polymorphic_call_context ctx;
	  ctx = ipa_context_from_jfunc (IPA_NODE_REF (cs->caller), cs, i,
					jfunc);
	  if (first)
	    {
	      newval = ctx;
	      first = false;
	    }
	  else
	    newval.meet_with (ctx);
	  if (newval.useless_p ())
	    break;
	}

      if (!newval.useless_p ())
	{
	  if (dump_file && (dump_flags & TDF_DETAILS))
	    {
	      fprintf (dump_file, "    adding an extra known polymorphic "
		       "context ");
	      print_ipcp_constant_value (dump_file, newval);
	      fprintf (dump_file, " for ");
	      ipa_dump_param (dump_file, info, i);
	      fprintf (dump_file, "\n");
	    }

	  if (!known_contexts->exists ())
	    known_contexts->safe_grow_cleared (ipa_get_param_count (info));
	  (*known_contexts)[i] = newval;
	}

    }
}