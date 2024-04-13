  Buf getBuf(const std::string& hex) {
    auto data = unhexlify(hex);
    return IOBuf::copyBuffer(data.data(), data.size());
  }