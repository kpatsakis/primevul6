true_always(node *tree)
{
  switch (nint(tree->car)) {
  case NODE_TRUE:
  case NODE_INT:
  case NODE_STR:
  case NODE_SYM:
    return TRUE;
  default:
    return FALSE;
  }
}