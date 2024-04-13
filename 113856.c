node_new_anychar(void)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  SET_NTYPE(node, NT_CANY);
  return node;
}