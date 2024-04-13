node_new_str(const UChar* s, const UChar* end)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  SET_NTYPE(node, NT_STR);
  NSTR(node)->capa = 0;
  NSTR(node)->flag = 0;
  NSTR(node)->s    = NSTR(node)->buf;
  NSTR(node)->end  = NSTR(node)->buf;
  if (onig_node_str_cat(node, s, end)) {
    onig_node_free(node);
    return NULL;
  }
  return node;
}