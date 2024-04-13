  ~PyTapeTensor() {
    if (shape_.index() == 1) {
      Py_DECREF(absl::get<1>(shape_));
    }
  }