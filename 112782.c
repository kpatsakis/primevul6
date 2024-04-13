const char *string_memnstr(const char *haystack, const char *needle,
                           int needle_len, const char *end) {
  const char *p = haystack;
  char ne = needle[needle_len-1];

  end -= needle_len;
  while (p <= end) {
    if ((p = (char *)memchr(p, *needle, (end-p+1))) && ne == p[needle_len-1]) {
      if (!memcmp(needle, p, needle_len-1)) {
        return p;
      }
    }
    if (p == nullptr) {
      return nullptr;
    }
    p++;
  }
  return nullptr;
}