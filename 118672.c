  uint32_t readListBegin(TType& elemType, uint32_t& size, bool& sizeUnknown) {
    T_VIRTUAL_CALL();
    return readListBegin_virt(elemType, size, sizeUnknown);
  }