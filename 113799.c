onig_node_str_set(Node* node, const UChar* s, const UChar* end)
{
  onig_node_str_clear(node);
  return onig_node_str_cat(node, s, end);
}