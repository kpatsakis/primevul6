onig_node_conv_to_str_node(Node* node, int flag)
{
  SET_NTYPE(node, NT_STR);
  NSTR(node)->flag = flag;
  NSTR(node)->capa = 0;
  NSTR(node)->s    = NSTR(node)->buf;
  NSTR(node)->end  = NSTR(node)->buf;
}