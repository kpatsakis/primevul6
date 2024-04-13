parse_branch(Node** top, OnigToken* tok, int term,
	     UChar** src, UChar* end, ScanEnv* env)
{
  int r;
  Node *node, **headp;

  *top = NULL;
  r = parse_exp(&node, tok, term, src, end, env);
  if (r < 0) {
    onig_node_free(node);
    return r;
  }

  if (r == TK_EOT || r == term || r == TK_ALT) {
    *top = node;
  }
  else {
    *top  = node_new_list(node, NULL);
    headp = &(NCDR(*top));
    while (r != TK_EOT && r != term && r != TK_ALT) {
      r = parse_exp(&node, tok, term, src, end, env);
      if (r < 0) {
	onig_node_free(node);
	return r;
      }

      if (NTYPE(node) == NT_LIST) {
        *headp = node;
        while (IS_NOT_NULL(NCDR(node))) node = NCDR(node);
        headp = &(NCDR(node));
      }
      else {
        *headp = node_new_list(node, NULL);
        headp = &(NCDR(*headp));
      }
    }
  }

  return r;
}