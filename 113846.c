node_new_list(Node* left, Node* right)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  SET_NTYPE(node, NT_LIST);
  NCAR(node)  = left;
  NCDR(node) = right;
  return node;
}