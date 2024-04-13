PyObject* TFE_Py_TapeSetIsStopped() {
  if (*ThreadTapeIsStopped()) {
    Py_RETURN_TRUE;
  }
  Py_RETURN_FALSE;
}