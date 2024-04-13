PyObject* TFE_Py_RegisterJVPFunction(PyObject* e) {
  if (forward_gradient_function != nullptr) {
    Py_DECREF(forward_gradient_function);
  }
  if (!PyCallable_Check(e)) {
    forward_gradient_function = nullptr;
    PyErr_SetString(PyExc_TypeError,
                    "TFE_Py_RegisterJVPFunction: "
                    "Registered object should be function.");
    return nullptr;
  } else {
    Py_INCREF(e);
    forward_gradient_function = e;
    Py_RETURN_NONE;
  }
}