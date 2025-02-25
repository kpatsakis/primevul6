  PyTapeTensor(const PyTapeTensor& other) {
    id_ = other.id_;
    dtype_ = other.dtype_;
    shape_ = other.shape_;
    if (shape_.index() == 1) {
      Py_INCREF(absl::get<1>(shape_));
    }
  }