  PyObject* ZerosLike(PyObject* tensor) const {
    if (PyErr_Occurred()) {
      return nullptr;
    }
    return PyObject_CallFunctionObjArgs(zeros_like_fn_, tensor, NULL);
  }