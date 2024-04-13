  uint32_t readBinary(folly::fbstring& str) {
    T_VIRTUAL_CALL();
    std::string data;
    uint32_t ret = readBinary_virt(data);
    str = data;
    return ret;
  }