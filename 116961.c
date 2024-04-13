Py_ssize_t TensorShapeNumDims(PyObject* value) {
  const auto size = PySequence_Size(value);
  if (size == -1) {
    // TensorShape.__len__ raises an error in the scenario where the shape is an
    // unknown, which needs to be cleared.
    // TODO(nareshmodi): ensure that this is actually a TensorShape.
    PyErr_Clear();
  }
  return size;
}