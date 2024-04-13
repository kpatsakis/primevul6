PyObject* TFE_Py_TapeSetIsEmpty() {
  if (*ThreadTapeIsStopped() || !HasAccumulatorOrTape()) {
    Py_RETURN_TRUE;
  }
  Py_RETURN_FALSE;
}