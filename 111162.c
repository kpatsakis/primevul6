false_always(node *tree)
{
  switch (nint(tree->car)) {
  case NODE_FALSE:
  case NODE_NIL:
    return TRUE;
  default:
    return FALSE;
  }
}