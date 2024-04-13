find_more_scalar_values_for_callers_subset (struct cgraph_node *node,
					    vec<tree> known_csts,
					    vec<cgraph_edge *> callers)
{
  class ipa_node_params *info = IPA_NODE_REF (node);
  int i, count = ipa_get_param_count (info);

  for (i = 0; i < count; i++)
    {
      struct cgraph_edge *cs;
      tree newval = NULL_TREE;
      int j;
      bool first = true;
      tree type = ipa_get_type (info, i);

      if (ipa_get_scalar_lat (info, i)->bottom || known_csts[i])
	continue;

      FOR_EACH_VEC_ELT (callers, j, cs)
	{
	  struct ipa_jump_func *jump_func;
	  tree t;

	  if (IPA_NODE_REF (cs->caller) && IPA_NODE_REF (cs->caller)->node_dead)
	    continue;

	  if (!IPA_EDGE_REF (cs)
	      || i >= ipa_get_cs_argument_count (IPA_EDGE_REF (cs))
	      || (i == 0
		  && call_passes_through_thunk_p (cs)))
	    {
	      newval = NULL_TREE;
	      break;
	    }
	  jump_func = ipa_get_ith_jump_func (IPA_EDGE_REF (cs), i);
	  if (self_recursive_pass_through_p (cs, jump_func, i))
	    continue;

	  t = ipa_value_from_jfunc (IPA_NODE_REF (cs->caller), jump_func, type);
	  if (!t
	      || (newval
		  && !values_equal_for_ipcp_p (t, newval))
	      || (!first && !newval))
	    {
	      newval = NULL_TREE;
	      break;
	    }
	  else
	    newval = t;
	  first = false;
	}

      if (newval)
	{
	  if (dump_file && (dump_flags & TDF_DETAILS))
	    {
	      fprintf (dump_file, "    adding an extra known scalar value ");
	      print_ipcp_constant_value (dump_file, newval);
	      fprintf (dump_file, " for ");
	      ipa_dump_param (dump_file, info, i);
	      fprintf (dump_file, "\n");
	    }

	  known_csts[i] = newval;
	}
    }
}