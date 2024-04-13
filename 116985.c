bool IsInteger(PyObject* py_value) {
#if PY_MAJOR_VERSION >= 3
  return PyLong_Check(py_value);
#else
  return PyInt_Check(py_value) || PyLong_Check(py_value);
#endif
}