int mg_strcmp(const struct mg_str str1, const struct mg_str str2) {
  size_t i = 0;
  while (i < str1.len && i < str2.len) {
    int c1 = str1.ptr[i];
    int c2 = str2.ptr[i];
    if (c1 < c2) return -1;
    if (c1 > c2) return 1;
    i++;
  }
  if (i < str1.len) return 1;
  if (i < str2.len) return -1;
  return 0;
}