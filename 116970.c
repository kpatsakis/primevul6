  PyObject* GraphShape(PyObject* tensor) const {
    PyObject* arg_list = Py_BuildValue("(O)", tensor);
    PyObject* result = PyEval_CallObject(graph_shape_fn_, arg_list);
    Py_DECREF(arg_list);
    return result;
  }