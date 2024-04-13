  explicit SafeSetCopy(const ContainerType& to_copy) : set_copy_(to_copy) {
    for (auto* member : set_copy_) {
      Py_INCREF(member);
    }
  }