  ~SafeSetCopy() {
    for (auto* member : set_copy_) {
      Py_DECREF(member);
    }
  }