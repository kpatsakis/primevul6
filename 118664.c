  uint32_t writeString(const folly::fbstring& str) {
    T_VIRTUAL_CALL();
    return writeString_virt(str.toStdString());
  }