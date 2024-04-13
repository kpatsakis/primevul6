  bool IsFixedSizeType(DataType dtype) {
    return dtype != DT_STRING && dtype != DT_VARIANT && dtype != DT_RESOURCE &&
           !kQuantizedTypes.Contains(dtype);
  }