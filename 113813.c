node_new_str_raw_char(UChar c)
{
  UChar p[1];

  p[0] = c;
  return node_new_str_raw(p, p + 1);
}