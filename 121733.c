  static bool isEqual(
      const std::unique_ptr<folly::IOBuf>& lhs,
      const std::unique_ptr<folly::IOBuf>& rhs) {
    return folly::IOBufEqualTo{}(lhs, rhs);
  }