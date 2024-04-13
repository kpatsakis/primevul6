static struct mg_str stripquotes(struct mg_str s) {
  return s.len > 1 && s.ptr[0] == '"' && s.ptr[s.len - 1] == '"'
             ? mg_str_n(s.ptr + 1, s.len - 2)
             : s;
}