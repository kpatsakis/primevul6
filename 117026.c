  PyTapeTensor TapeTensorFromGradient(PyObject* tensor) const final {
    return TapeTensorFromTensor(tensor);
  }