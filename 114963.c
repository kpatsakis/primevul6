propagate_constants_across_call (struct cgraph_edge *cs)
{
  class ipa_node_params *callee_info;
  enum availability availability;
  cgraph_node *callee;
  class ipa_edge_args *args;
  bool ret = false;
  int i, args_count, parms_count;

  callee = cs->callee->function_symbol (&availability);
  if (!callee->definition)
    return false;
  gcc_checking_assert (callee->has_gimple_body_p ());
  callee_info = IPA_NODE_REF (callee);
  if (!callee_info)
    return false;

  args = IPA_EDGE_REF (cs);
  parms_count = ipa_get_param_count (callee_info);
  if (parms_count == 0)
    return false;
  if (!args
      || !opt_for_fn (cs->caller->decl, flag_ipa_cp)
      || !opt_for_fn (cs->caller->decl, optimize))
    {
      for (i = 0; i < parms_count; i++)
	ret |= set_all_contains_variable (ipa_get_parm_lattices (callee_info,
								 i));
      return ret;
    }
  args_count = ipa_get_cs_argument_count (args);

  /* If this call goes through a thunk we must not propagate to the first (0th)
     parameter.  However, we might need to uncover a thunk from below a series
     of aliases first.  */
  if (call_passes_through_thunk_p (cs))
    {
      ret |= set_all_contains_variable (ipa_get_parm_lattices (callee_info,
							       0));
      i = 1;
    }
  else
    i = 0;

  for (; (i < args_count) && (i < parms_count); i++)
    {
      struct ipa_jump_func *jump_func = ipa_get_ith_jump_func (args, i);
      class ipcp_param_lattices *dest_plats;
      tree param_type = ipa_get_type (callee_info, i);

      dest_plats = ipa_get_parm_lattices (callee_info, i);
      if (availability == AVAIL_INTERPOSABLE)
	ret |= set_all_contains_variable (dest_plats);
      else
	{
	  ret |= propagate_scalar_across_jump_function (cs, jump_func,
							&dest_plats->itself,
							param_type);
	  ret |= propagate_context_across_jump_function (cs, jump_func, i,
							 &dest_plats->ctxlat);
	  ret
	    |= propagate_bits_across_jump_function (cs, i, jump_func,
						    &dest_plats->bits_lattice);
	  ret |= propagate_aggs_across_jump_function (cs, jump_func,
						      dest_plats);
	  if (opt_for_fn (callee->decl, flag_ipa_vrp))
	    ret |= propagate_vr_across_jump_function (cs, jump_func,
						      dest_plats, param_type);
	  else
	    ret |= dest_plats->m_value_range.set_to_bottom ();
	}
    }
  for (; i < parms_count; i++)
    ret |= set_all_contains_variable (ipa_get_parm_lattices (callee_info, i));

  return ret;
}