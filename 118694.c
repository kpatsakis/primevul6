  virtual uint32_t skip_virt(TType type) {
    return ::apache::thrift::protocol::skip(*this, type);
  }