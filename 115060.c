values_equal_for_ipcp_p (tree x, tree y)
{
  gcc_checking_assert (x != NULL_TREE && y != NULL_TREE);

  if (x == y)
    return true;

  if (TREE_CODE (x) ==  ADDR_EXPR
      && TREE_CODE (y) ==  ADDR_EXPR
      && TREE_CODE (TREE_OPERAND (x, 0)) == CONST_DECL
      && TREE_CODE (TREE_OPERAND (y, 0)) == CONST_DECL)
    return operand_equal_p (DECL_INITIAL (TREE_OPERAND (x, 0)),
			    DECL_INITIAL (TREE_OPERAND (y, 0)), 0);
  else
    return operand_equal_p (x, y, 0);
}