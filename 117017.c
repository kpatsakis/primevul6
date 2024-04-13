  PyObject* Ones(PyObject* shape, PyObject* dtype) const {
    if (PyErr_Occurred()) {
      return nullptr;
    }
    PyObject* arg_list = Py_BuildValue("OO", shape, dtype);
    PyObject* result = PyEval_CallObject(ones_fn_, arg_list);
    Py_DECREF(arg_list);
    return result;
  }