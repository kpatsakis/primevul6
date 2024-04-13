onig_node_list_add(Node* list, Node* x)
{
  Node *n;

  n = onig_node_new_list(x, NULL);
  if (IS_NULL(n)) return NULL_NODE;

  if (IS_NOT_NULL(list)) {
    while (IS_NOT_NULL(NCDR(list)))
      list = NCDR(list);

    NCDR(list) = n;
  }

  return n;
}