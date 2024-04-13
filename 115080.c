propagate_context_across_jump_function (cgraph_edge *cs,
			  ipa_jump_func *jfunc, int idx,
			  ipcp_lattice<ipa_polymorphic_call_context> *dest_lat)
{
  ipa_edge_args *args = IPA_EDGE_REF (cs);
  if (dest_lat->bottom)
    return false;
  bool ret = false;
  bool added_sth = false;
  bool type_preserved = true;

  ipa_polymorphic_call_context edge_ctx, *edge_ctx_ptr
    = ipa_get_ith_polymorhic_call_context (args, idx);

  if (edge_ctx_ptr)
    edge_ctx = *edge_ctx_ptr;

  if (jfunc->type == IPA_JF_PASS_THROUGH
      || jfunc->type == IPA_JF_ANCESTOR)
    {
      class ipa_node_params *caller_info = IPA_NODE_REF (cs->caller);
      int src_idx;
      ipcp_lattice<ipa_polymorphic_call_context> *src_lat;

      /* TODO: Once we figure out how to propagate speculations, it will
	 probably be a good idea to switch to speculation if type_preserved is
	 not set instead of punting.  */
      if (jfunc->type == IPA_JF_PASS_THROUGH)
	{
	  if (ipa_get_jf_pass_through_operation (jfunc) != NOP_EXPR)
	    goto prop_fail;
	  type_preserved = ipa_get_jf_pass_through_type_preserved (jfunc);
	  src_idx = ipa_get_jf_pass_through_formal_id (jfunc);
	}
      else
	{
	  type_preserved = ipa_get_jf_ancestor_type_preserved (jfunc);
	  src_idx = ipa_get_jf_ancestor_formal_id (jfunc);
	}

      src_lat = ipa_get_poly_ctx_lat (caller_info, src_idx);
      /* If we would need to clone the caller and cannot, do not propagate.  */
      if (!ipcp_versionable_function_p (cs->caller)
	  && (src_lat->contains_variable
	      || (src_lat->values_count > 1)))
	goto prop_fail;

      ipcp_value<ipa_polymorphic_call_context> *src_val;
      for (src_val = src_lat->values; src_val; src_val = src_val->next)
	{
	  ipa_polymorphic_call_context cur = src_val->value;

	  if (!type_preserved)
	    cur.possible_dynamic_type_change (cs->in_polymorphic_cdtor);
	  if (jfunc->type == IPA_JF_ANCESTOR)
	    cur.offset_by (ipa_get_jf_ancestor_offset (jfunc));
	  /* TODO: In cases we know how the context is going to be used,
	     we can improve the result by passing proper OTR_TYPE.  */
	  cur.combine_with (edge_ctx);
	  if (!cur.useless_p ())
	    {
	      if (src_lat->contains_variable
		  && !edge_ctx.equal_to (cur))
		ret |= dest_lat->set_contains_variable ();
	      ret |= dest_lat->add_value (cur, cs, src_val, src_idx);
	      added_sth = true;
	    }
	}
    }

 prop_fail:
  if (!added_sth)
    {
      if (!edge_ctx.useless_p ())
	ret |= dest_lat->add_value (edge_ctx, cs);
      else
	ret |= dest_lat->set_contains_variable ();
    }

  return ret;
}