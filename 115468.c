inline bool decode_codepoint(const char *s8, size_t l, size_t &bytes,
                             char32_t &cp) {
  if (l) {
    auto b = static_cast<uint8_t>(s8[0]);
    if ((b & 0x80) == 0) {
      bytes = 1;
      cp = b;
      return true;
    } else if ((b & 0xE0) == 0xC0) {
      if (l >= 2) {
        bytes = 2;
        cp = ((static_cast<char32_t>(s8[0] & 0x1F)) << 6) |
             (static_cast<char32_t>(s8[1] & 0x3F));
        return true;
      }
    } else if ((b & 0xF0) == 0xE0) {
      if (l >= 3) {
        bytes = 3;
        cp = ((static_cast<char32_t>(s8[0] & 0x0F)) << 12) |
             ((static_cast<char32_t>(s8[1] & 0x3F)) << 6) |
             (static_cast<char32_t>(s8[2] & 0x3F));
        return true;
      }
    } else if ((b & 0xF8) == 0xF0) {
      if (l >= 4) {
        bytes = 4;
        cp = ((static_cast<char32_t>(s8[0] & 0x07)) << 18) |
             ((static_cast<char32_t>(s8[1] & 0x3F)) << 12) |
             ((static_cast<char32_t>(s8[2] & 0x3F)) << 6) |
             (static_cast<char32_t>(s8[3] & 0x3F));
        return true;
      }
    }
  }
  return false;
}