  PyObject* OnesLike(PyObject* tensor) const {
    if (PyErr_Occurred()) {
      return nullptr;
    }
    return PyObject_CallFunctionObjArgs(ones_like_fn_, tensor, NULL);
  }