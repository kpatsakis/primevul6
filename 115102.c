propagate_vr_across_jump_function (cgraph_edge *cs, ipa_jump_func *jfunc,
				   class ipcp_param_lattices *dest_plats,
				   tree param_type)
{
  ipcp_vr_lattice *dest_lat = &dest_plats->m_value_range;

  if (dest_lat->bottom_p ())
    return false;

  if (!param_type
      || (!INTEGRAL_TYPE_P (param_type)
	  && !POINTER_TYPE_P (param_type)))
    return dest_lat->set_to_bottom ();

  if (jfunc->type == IPA_JF_PASS_THROUGH)
    {
      enum tree_code operation = ipa_get_jf_pass_through_operation (jfunc);
      class ipa_node_params *caller_info = IPA_NODE_REF (cs->caller);
      int src_idx = ipa_get_jf_pass_through_formal_id (jfunc);
      class ipcp_param_lattices *src_lats
	= ipa_get_parm_lattices (caller_info, src_idx);
      tree operand_type = ipa_get_type (caller_info, src_idx);

      if (src_lats->m_value_range.bottom_p ())
	return dest_lat->set_to_bottom ();

      value_range vr;
      if (TREE_CODE_CLASS (operation) == tcc_unary)
	ipa_vr_operation_and_type_effects (&vr,
					   &src_lats->m_value_range.m_vr,
					   operation, param_type,
					   operand_type);
      /* A crude way to prevent unbounded number of value range updates
	 in SCC components.  We should allow limited number of updates within
	 SCC, too.  */
      else if (!ipa_edge_within_scc (cs))
	{
	  tree op = ipa_get_jf_pass_through_operand (jfunc);
	  value_range op_vr (op, op);
	  value_range op_res,res;

	  range_fold_binary_expr (&op_res, operation, operand_type,
				  &src_lats->m_value_range.m_vr, &op_vr);
	  ipa_vr_operation_and_type_effects (&vr,
					     &op_res,
					     NOP_EXPR, param_type,
					     operand_type);
	}
      if (!vr.undefined_p () && !vr.varying_p ())
	{
	  if (jfunc->m_vr)
	    {
	      value_range jvr;
	      if (ipa_vr_operation_and_type_effects (&jvr, jfunc->m_vr,
						     NOP_EXPR,
						     param_type,
						     jfunc->m_vr->type ()))
		vr.intersect (jvr);
	    }
	  return dest_lat->meet_with (&vr);
	}
    }
  else if (jfunc->type == IPA_JF_CONST)
    {
      tree val = ipa_get_jf_constant (jfunc);
      if (TREE_CODE (val) == INTEGER_CST)
	{
	  val = fold_convert (param_type, val);
	  if (TREE_OVERFLOW_P (val))
	    val = drop_tree_overflow (val);

	  value_range tmpvr (val, val);
	  return dest_lat->meet_with (&tmpvr);
	}
    }

  value_range vr;
  if (jfunc->m_vr
      && ipa_vr_operation_and_type_effects (&vr, jfunc->m_vr, NOP_EXPR,
					    param_type,
					    jfunc->m_vr->type ()))
    return dest_lat->meet_with (&vr);
  else
    return dest_lat->set_to_bottom ();
}