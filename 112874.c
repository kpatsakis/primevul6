folly::Optional<int> maxEncodedSize(int length) {
  if ((length + 2) < 0 || ((length + 2) / 3) >= (1 << (sizeof(int) * 8 - 2))) {
    return folly::none;
  }
  return ((length + 2) / 3) * 4;
}