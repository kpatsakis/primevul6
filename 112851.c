int string_rfind(const char *input, int len, char ch, int pos,
                 bool case_sensitive) {
  assertx(input);
  if (pos < -len || pos > len) {
    return -1;
  }
  const void *ptr;
  if (case_sensitive) {
    if (pos >= 0) {
      ptr = memrchr(input + pos, ch, len - pos);
    } else {
      ptr = memrchr(input, ch, len + pos + 1);
    }
  } else {
    if (pos >= 0) {
      ptr = bstrrcasechr(input + pos, ch, len - pos);
    } else {
      ptr = bstrrcasechr(input, ch, len + pos + 1);
    }
  }
  if (ptr != nullptr) {
    return (int)((const char *)ptr - input);
  }
  return -1;
}