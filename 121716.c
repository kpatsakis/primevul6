  static std::unique_ptr<folly::IOBuf> fromStringLiteral(const char* str) {
    return (
        str[0] != '\0' ? folly::IOBuf::wrapBuffer(str, strlen(str)) : nullptr);
  }