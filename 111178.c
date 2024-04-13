gen_retval(codegen_scope *s, node *tree)
{
  if (nint(tree->car) == NODE_SPLAT) {
    codegen(s, tree, VAL);
    pop();
    genop_1(s, OP_ARYDUP, cursp());
  }
  else {
    codegen(s, tree, VAL);
    pop();
  }
}