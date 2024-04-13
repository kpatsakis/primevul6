print_ipcp_constant_value (FILE * f, tree v)
{
  if (TREE_CODE (v) == ADDR_EXPR
      && TREE_CODE (TREE_OPERAND (v, 0)) == CONST_DECL)
    {
      fprintf (f, "& ");
      print_generic_expr (f, DECL_INITIAL (TREE_OPERAND (v, 0)));
    }
  else
    print_generic_expr (f, v);
}