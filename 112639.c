static int isUtf8AsciiLower(folly::StringPiece s) {
  const auto bytelen = s.size();
  bool caseOK = true;
  for (uint32_t i = 0; i < bytelen; ++i) {
    uint8_t byte = s[i];
    if (byte >= 128) {
      return 0;
    } else if (byte <= 'Z' && byte >= 'A') {
      caseOK = false;
    }
  }
  return caseOK ? 1 : -1;
}