int string_find(const char *input, int len, const char *s, int s_len,
                int pos, bool case_sensitive) {
  assertx(input);
  assertx(s);
  if (!s_len || pos < 0 || pos > len) {
    return -1;
  }
  void *ptr;
  if (case_sensitive) {
    ptr = (void*)string_memnstr(input + pos, s, s_len, input + len);
  } else {
    ptr = bstrcasestr(input + pos, len - pos, s, s_len);
  }
  if (ptr != nullptr) {
    return (int)((const char *)ptr - input);
  }
  return -1;
}