  static bool isLess(const folly::IOBuf& lhs, const folly::IOBuf& rhs) {
    return folly::IOBufLess{}(lhs, rhs);
  }