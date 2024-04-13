  static bool isEqual(const folly::IOBuf& lhs, const folly::IOBuf& rhs) {
    return folly::IOBufEqualTo{}(lhs, rhs);
  }