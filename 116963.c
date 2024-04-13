PyObject* PyTapeTensor::GetShape() const {
  if (shape_.index() == 0) {
    auto& shape = absl::get<0>(shape_);
    PyObject* py_shape = PyTuple_New(shape.dims());
    for (int i = 0; i < shape.dims(); ++i) {
      PyTuple_SET_ITEM(py_shape, i, PyLong_FromLong(shape.dim_size(i)));
    }

    return py_shape;
  }

  return py_vspace->GraphShape(absl::get<1>(shape_));
}