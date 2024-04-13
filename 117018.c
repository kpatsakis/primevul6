PyObject* TFE_Py_RegisterFallbackExceptionClass(PyObject* e) {
  if (fallback_exception_class != nullptr) {
    Py_DECREF(fallback_exception_class);
  }
  if (PyObject_IsSubclass(e, PyExc_Exception) <= 0) {
    fallback_exception_class = nullptr;
    PyErr_SetString(PyExc_TypeError,
                    "TFE_Py_RegisterFallbackExceptionClass: "
                    "Registered class should be subclass of Exception.");
    return nullptr;
  } else {
    Py_INCREF(e);
    fallback_exception_class = e;
    Py_RETURN_NONE;
  }
}