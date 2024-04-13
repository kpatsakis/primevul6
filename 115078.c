get_val_across_arith_op (enum tree_code opcode,
			 tree opnd1_type,
			 tree opnd2,
			 ipcp_value<tree> *src_val,
			 tree res_type)
{
  tree opnd1 = src_val->value;

  /* Skip source values that is incompatible with specified type.  */
  if (opnd1_type
      && !useless_type_conversion_p (opnd1_type, TREE_TYPE (opnd1)))
    return NULL_TREE;

  return ipa_get_jf_arith_result (opcode, opnd1, opnd2, res_type);
}