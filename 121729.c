  static folly::IOBuf fromStringLiteral(const char* str) {
    return folly::IOBuf::wrapBufferAsValue(str, strlen(str));
  }