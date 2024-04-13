node_str_cat_char(Node* node, UChar c)
{
  UChar s[1];

  s[0] = c;
  return onig_node_str_cat(node, s, s + 1);
}