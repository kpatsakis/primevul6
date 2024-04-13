  explicit PyVSpace(PyObject* py_vspace) : py_vspace_(py_vspace) {
    Py_INCREF(py_vspace_);
  }