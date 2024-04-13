propagate_bits_across_jump_function (cgraph_edge *cs, int idx,
				     ipa_jump_func *jfunc,
				     ipcp_bits_lattice *dest_lattice)
{
  if (dest_lattice->bottom_p ())
    return false;

  enum availability availability;
  cgraph_node *callee = cs->callee->function_symbol (&availability);
  class ipa_node_params *callee_info = IPA_NODE_REF (callee);
  tree parm_type = ipa_get_type (callee_info, idx);

  /* For K&R C programs, ipa_get_type() could return NULL_TREE.  Avoid the
     transform for these cases.  Similarly, we can have bad type mismatches
     with LTO, avoid doing anything with those too.  */
  if (!parm_type
      || (!INTEGRAL_TYPE_P (parm_type) && !POINTER_TYPE_P (parm_type)))
    {
      if (dump_file && (dump_flags & TDF_DETAILS))
	fprintf (dump_file, "Setting dest_lattice to bottom, because type of "
		 "param %i of %s is NULL or unsuitable for bits propagation\n",
		 idx, cs->callee->name ());

      return dest_lattice->set_to_bottom ();
    }

  unsigned precision = TYPE_PRECISION (parm_type);
  signop sgn = TYPE_SIGN (parm_type);

  if (jfunc->type == IPA_JF_PASS_THROUGH
      || jfunc->type == IPA_JF_ANCESTOR)
    {
      class ipa_node_params *caller_info = IPA_NODE_REF (cs->caller);
      tree operand = NULL_TREE;
      enum tree_code code;
      unsigned src_idx;

      if (jfunc->type == IPA_JF_PASS_THROUGH)
	{
	  code = ipa_get_jf_pass_through_operation (jfunc);
	  src_idx = ipa_get_jf_pass_through_formal_id (jfunc);
	  if (code != NOP_EXPR)
	    operand = ipa_get_jf_pass_through_operand (jfunc);
	}
      else
	{
	  code = POINTER_PLUS_EXPR;
	  src_idx = ipa_get_jf_ancestor_formal_id (jfunc);
	  unsigned HOST_WIDE_INT offset = ipa_get_jf_ancestor_offset (jfunc) / BITS_PER_UNIT;
	  operand = build_int_cstu (size_type_node, offset);
	}

      class ipcp_param_lattices *src_lats
	= ipa_get_parm_lattices (caller_info, src_idx);

      /* Try to propagate bits if src_lattice is bottom, but jfunc is known.
	 for eg consider:
	 int f(int x)
	 {
	   g (x & 0xff);
	 }
	 Assume lattice for x is bottom, however we can still propagate
	 result of x & 0xff == 0xff, which gets computed during ccp1 pass
	 and we store it in jump function during analysis stage.  */

      if (src_lats->bits_lattice.bottom_p ()
	  && jfunc->bits)
	return dest_lattice->meet_with (jfunc->bits->value, jfunc->bits->mask,
					precision);
      else
	return dest_lattice->meet_with (src_lats->bits_lattice, precision, sgn,
					code, operand);
    }

  else if (jfunc->type == IPA_JF_ANCESTOR)
    return dest_lattice->set_to_bottom ();
  else if (jfunc->bits)
    return dest_lattice->meet_with (jfunc->bits->value, jfunc->bits->mask,
				    precision);
  else
    return dest_lattice->set_to_bottom ();
}