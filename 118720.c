  uint32_t readString(folly::fbstring& str) {
    T_VIRTUAL_CALL();
    std::string data;
    uint32_t ret = readString_virt(data);
    str = data;
    return ret;
  }