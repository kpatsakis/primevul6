PyObject* PyTapeTensor::OnesLike() const {
  if (shape_.index() == 1) {
    PyObject* tensor = absl::get<1>(shape_);
    return py_vspace->OnesLike(tensor);
  }
  PyObject* py_shape = GetShape();
  PyObject* dtype_field = GetPyDType();
  PyObject* result = py_vspace->Ones(py_shape, dtype_field);
  Py_DECREF(dtype_field);
  Py_DECREF(py_shape);
  return result;
}