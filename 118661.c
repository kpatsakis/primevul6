  uint32_t readSetBegin(TType& elemType, uint32_t& size, bool& sizeUnknown) {
    T_VIRTUAL_CALL();
    return readSetBegin_virt(elemType, size, sizeUnknown);
  }