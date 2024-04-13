ipa_context_from_jfunc (ipa_node_params *info, cgraph_edge *cs, int csidx,
			ipa_jump_func *jfunc)
{
  ipa_edge_args *args = IPA_EDGE_REF (cs);
  ipa_polymorphic_call_context ctx;
  ipa_polymorphic_call_context *edge_ctx
    = cs ? ipa_get_ith_polymorhic_call_context (args, csidx) : NULL;

  if (edge_ctx && !edge_ctx->useless_p ())
    ctx = *edge_ctx;

  if (jfunc->type == IPA_JF_PASS_THROUGH
      || jfunc->type == IPA_JF_ANCESTOR)
    {
      ipa_polymorphic_call_context srcctx;
      int srcidx;
      bool type_preserved = true;
      if (jfunc->type == IPA_JF_PASS_THROUGH)
	{
	  if (ipa_get_jf_pass_through_operation (jfunc) != NOP_EXPR)
	    return ctx;
	  type_preserved = ipa_get_jf_pass_through_type_preserved (jfunc);
	  srcidx = ipa_get_jf_pass_through_formal_id (jfunc);
	}
      else
	{
	  type_preserved = ipa_get_jf_ancestor_type_preserved (jfunc);
	  srcidx = ipa_get_jf_ancestor_formal_id (jfunc);
	}
      if (info->ipcp_orig_node)
	{
	  if (info->known_contexts.exists ())
	    srcctx = info->known_contexts[srcidx];
	}
      else
	{
	  if (!info->lattices
	      || srcidx >= ipa_get_param_count (info))
	    return ctx;
	  ipcp_lattice<ipa_polymorphic_call_context> *lat;
	  lat = ipa_get_poly_ctx_lat (info, srcidx);
	  if (!lat->is_single_const ())
	    return ctx;
	  srcctx = lat->values->value;
	}
      if (srcctx.useless_p ())
	return ctx;
      if (jfunc->type == IPA_JF_ANCESTOR)
	srcctx.offset_by (ipa_get_jf_ancestor_offset (jfunc));
      if (!type_preserved)
	srcctx.possible_dynamic_type_change (cs->in_polymorphic_cdtor);
      srcctx.combine_with (ctx);
      return srcctx;
    }

  return ctx;
}