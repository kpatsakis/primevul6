static inline char getSpecialChar(char c) {
  // Optimized for code size on a 8-bit AVR

  const char *p = specialChars;

  while (p[0] && p[1] != c) {
    p += 2;
  }

  return p[0];
}