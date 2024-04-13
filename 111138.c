node_len(node *tree)
{
  int n = 0;

  while (tree) {
    n++;
    tree = tree->cdr;
  }
  return n;
}