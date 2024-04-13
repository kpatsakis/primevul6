inline std::u32string decode(const char *s8, size_t l) {
  std::u32string out;
  size_t i = 0;
  while (i < l) {
    auto beg = i++;
    while (i < l && (s8[i] & 0xc0) == 0x80) {
      i++;
    }
    out += decode_codepoint(&s8[beg], (i - beg));
  }
  return out;
}