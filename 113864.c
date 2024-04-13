onig_node_new_anchor(int type)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  SET_NTYPE(node, NT_ANCHOR);
  NANCHOR(node)->type     = type;
  NANCHOR(node)->target   = NULL;
  NANCHOR(node)->char_len = -1;
  return node;
}