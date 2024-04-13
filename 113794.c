node_new_call(UChar* name, UChar* name_end, int gnum)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  SET_NTYPE(node, NT_CALL);
  NCALL(node)->state     = 0;
  NCALL(node)->target    = NULL_NODE;
  NCALL(node)->name      = name;
  NCALL(node)->name_end  = name_end;
  NCALL(node)->group_num = gnum;  /* call by number if gnum != 0 */
  return node;
}