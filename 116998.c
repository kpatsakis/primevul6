static void TFE_Py_Tape_Delete(PyObject* tape) {
  delete reinterpret_cast<TFE_Py_Tape*>(tape)->tape;
  Py_TYPE(tape)->tp_free(tape);
}