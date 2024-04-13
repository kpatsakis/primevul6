ipa_get_jf_arith_result (enum tree_code opcode, tree input, tree operand,
			 tree res_type)
{
  tree res;

  if (opcode == NOP_EXPR)
    return input;
  if (!is_gimple_ip_invariant (input))
    return NULL_TREE;

  if (!res_type)
    {
      if (TREE_CODE_CLASS (opcode) == tcc_comparison)
	res_type = boolean_type_node;
      else if (expr_type_first_operand_type_p (opcode))
	res_type = TREE_TYPE (input);
      else
	return NULL_TREE;
    }

  if (TREE_CODE_CLASS (opcode) == tcc_unary)
    res = fold_unary (opcode, res_type, input);
  else
    res = fold_binary (opcode, res_type, input, operand);

  if (res && !is_gimple_ip_invariant (res))
    return NULL_TREE;

  return res;
}