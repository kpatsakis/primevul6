PyObject* GetPythonObjectFromInt(int num) {
#if PY_MAJOR_VERSION >= 3
  return PyLong_FromLong(num);
#else
  return PyInt_FromLong(num);
#endif
}