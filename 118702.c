  uint32_t writeBinary(const folly::fbstring& str) {
    T_VIRTUAL_CALL();
    return writeBinary_virt(str.toStdString());
  }