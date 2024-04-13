  void *memrchr(const void *s, int c, size_t n) {
    for (const char *p = (const char *)s + n - 1; p >= s; p--) {
      if (*p == c) return (void *)p;
    }
    return nullptr;
  }