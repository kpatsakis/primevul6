  static bool isEmpty(const std::unique_ptr<folly::IOBuf>& str) {
    return !str || str->empty();
  }