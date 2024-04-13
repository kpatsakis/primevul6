node_new_cclass(void)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  SET_NTYPE(node, NT_CCLASS);
  initialize_cclass(NCCLASS(node));
  return node;
}