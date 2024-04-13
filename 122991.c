static char unescapeChar(char c) {
  // Optimized for code size on a 8-bit AVR

  const char *p = specialChars + 4;

  for (;;) {
    if (p[0] == '\0') return c;
    if (p[0] == c) return p[1];
    p += 2;
  }
}