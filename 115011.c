ipa_value_range_from_jfunc (ipa_node_params *info, cgraph_edge *cs,
			    ipa_jump_func *jfunc, tree parm_type)
{
  value_range vr;
  return vr;
  if (jfunc->m_vr)
    ipa_vr_operation_and_type_effects (&vr,
				       jfunc->m_vr,
				       NOP_EXPR, parm_type,
				       jfunc->m_vr->type ());
  if (vr.singleton_p ())
    return vr;
  if (jfunc->type == IPA_JF_PASS_THROUGH)
    {
      int idx;
      ipcp_transformation *sum
	= ipcp_get_transformation_summary (cs->caller->inlined_to
					   ? cs->caller->inlined_to
					   : cs->caller);
      if (!sum || !sum->m_vr)
	return vr;

      idx = ipa_get_jf_pass_through_formal_id (jfunc);

      if (!(*sum->m_vr)[idx].known)
	return vr;
      tree vr_type = ipa_get_type (info, idx);
      value_range srcvr (wide_int_to_tree (vr_type, (*sum->m_vr)[idx].min),
			 wide_int_to_tree (vr_type, (*sum->m_vr)[idx].max),
			 (*sum->m_vr)[idx].type);

      enum tree_code operation = ipa_get_jf_pass_through_operation (jfunc);

      if (TREE_CODE_CLASS (operation) == tcc_unary)
	{
	  value_range res;

	  if (ipa_vr_operation_and_type_effects (&res,
						 &srcvr,
						 operation, parm_type,
						 vr_type))
	    vr.intersect (res);
	}
      else
	{
	  value_range op_res, res;
	  tree op = ipa_get_jf_pass_through_operand (jfunc);
	  value_range op_vr (op, op);

	  range_fold_binary_expr (&op_res, operation, vr_type, &srcvr, &op_vr);
	  if (ipa_vr_operation_and_type_effects (&res,
						 &op_res,
						 NOP_EXPR, parm_type,
						 vr_type))
	    vr.intersect (res);
	}
    }
  return vr;
}