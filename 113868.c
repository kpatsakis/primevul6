onig_node_str_clear(Node* node)
{
  if (NSTR(node)->capa != 0 &&
      IS_NOT_NULL(NSTR(node)->s) && NSTR(node)->s != NSTR(node)->buf) {
    xfree(NSTR(node)->s);
  }

  NSTR(node)->capa = 0;
  NSTR(node)->flag = 0;
  NSTR(node)->s    = NSTR(node)->buf;
  NSTR(node)->end  = NSTR(node)->buf;
}