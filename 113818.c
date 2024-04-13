onig_node_new_alt(Node* left, Node* right)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  SET_NTYPE(node, NT_ALT);
  NCAR(node)  = left;
  NCDR(node) = right;
  return node;
}