int string_find(const char *input, int len, char ch, int pos,
                bool case_sensitive) {
  assertx(input);
  if (pos < 0 || pos > len) {
    return -1;
  }
  const void *ptr;
  if (case_sensitive) {
    ptr = memchr(input + pos, ch, len - pos);
  } else {
    ptr = bstrcasechr(input + pos, ch, len - pos);
  }
  if (ptr != nullptr) {
    return (int)((const char *)ptr - input);
  }
  return -1;
}