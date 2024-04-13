  PyTapeTensor(int64_t id, tensorflow::DataType dtype, PyObject* shape)
      : id_(id), dtype_(dtype), shape_(shape) {
    Py_INCREF(absl::get<1>(shape_));
  }