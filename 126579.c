struct mg_str mg_strstrip(struct mg_str s) {
  while (s.len > 0 && isspace((int) *s.ptr)) s.ptr++, s.len--;
  while (s.len > 0 && isspace((int) *(s.ptr + s.len - 1))) s.len--;
  return s;
}