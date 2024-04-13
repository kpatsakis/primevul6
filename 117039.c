PyObject* TFE_Py_RegisterGradientFunction(PyObject* e) {
  if (gradient_function != nullptr) {
    Py_DECREF(gradient_function);
  }
  if (!PyCallable_Check(e)) {
    gradient_function = nullptr;
    PyErr_SetString(PyExc_TypeError,
                    "TFE_Py_RegisterGradientFunction: "
                    "Registered object should be function.");
    return nullptr;
  } else {
    Py_INCREF(e);
    gradient_function = e;
    Py_RETURN_NONE;
  }
}