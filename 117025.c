  int64_t NumElements(PyObject* tensor) const final {
    if (EagerTensor_CheckExact(tensor)) {
      return PyEagerTensor_NumElements(tensor);
    }
    PyObject* arglist =
        Py_BuildValue("(O)", reinterpret_cast<PyObject*>(tensor));
    PyObject* result = PyEval_CallObject(num_elements_, arglist);
    Py_DECREF(arglist);
    if (result == nullptr) {
      // The caller detects whether a python exception has been raised.
      return -1;
    }
    int64_t r = MakeInt(result);
    Py_DECREF(result);
    return r;
  }