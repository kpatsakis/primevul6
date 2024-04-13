node_new_enclose(int type)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  SET_NTYPE(node, NT_ENCLOSE);
  NENCLOSE(node)->type      = type;
  NENCLOSE(node)->state     =  0;
  NENCLOSE(node)->regnum    =  0;
  NENCLOSE(node)->option    =  0;
  NENCLOSE(node)->target    = NULL;
  NENCLOSE(node)->call_addr = -1;
  NENCLOSE(node)->opt_count =  0;
  return node;
}