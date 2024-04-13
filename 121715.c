  static bool isLess(
      const std::unique_ptr<folly::IOBuf>& lhs,
      const std::unique_ptr<folly::IOBuf>& rhs) {
    return folly::IOBufLess{}(lhs, rhs);
  }