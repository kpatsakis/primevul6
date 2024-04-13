ipa_get_jf_ancestor_result (struct ipa_jump_func *jfunc, tree input)
{
  gcc_checking_assert (TREE_CODE (input) != TREE_BINFO);
  if (TREE_CODE (input) == ADDR_EXPR)
    {
      tree t = TREE_OPERAND (input, 0);
      t = build_ref_for_offset (EXPR_LOCATION (t), t,
				ipa_get_jf_ancestor_offset (jfunc), false,
				ptr_type_node, NULL, false);
      return build_fold_addr_expr (t);
    }
  else
    return NULL_TREE;
}