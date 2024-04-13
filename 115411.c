inline size_t encode_codepoint(char32_t cp, char *buff) {
  if (cp < 0x0080) {
    buff[0] = static_cast<char>(cp & 0x7F);
    return 1;
  } else if (cp < 0x0800) {
    buff[0] = static_cast<char>(0xC0 | ((cp >> 6) & 0x1F));
    buff[1] = static_cast<char>(0x80 | (cp & 0x3F));
    return 2;
  } else if (cp < 0xD800) {
    buff[0] = static_cast<char>(0xE0 | ((cp >> 12) & 0xF));
    buff[1] = static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
    buff[2] = static_cast<char>(0x80 | (cp & 0x3F));
    return 3;
  } else if (cp < 0xE000) {
    // D800 - DFFF is invalid...
    return 0;
  } else if (cp < 0x10000) {
    buff[0] = static_cast<char>(0xE0 | ((cp >> 12) & 0xF));
    buff[1] = static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
    buff[2] = static_cast<char>(0x80 | (cp & 0x3F));
    return 3;
  } else if (cp < 0x110000) {
    buff[0] = static_cast<char>(0xF0 | ((cp >> 18) & 0x7));
    buff[1] = static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
    buff[2] = static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
    buff[3] = static_cast<char>(0x80 | (cp & 0x3F));
    return 4;
  }
  return 0;
}