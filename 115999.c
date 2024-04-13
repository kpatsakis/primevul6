inline unsigned count_digits(uint32_t n) {
  int t = (32 - FMT_BUILTIN_CLZ(n | 1)) * 1233 >> 12;
  return to_unsigned(t) - (n < Data::POWERS_OF_10_32[t]) + 1;
}