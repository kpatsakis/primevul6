PyObject* TFE_Py_TapeSetNew(PyObject* persistent,
                            PyObject* watch_accessed_variables) {
  TFE_Py_Tape_Type.tp_new = PyType_GenericNew;
  if (PyType_Ready(&TFE_Py_Tape_Type) < 0) return nullptr;
  TFE_Py_Tape* tape = PyObject_NEW(TFE_Py_Tape, &TFE_Py_Tape_Type);
  tape->tape = new GradientTape(persistent == Py_True,
                                watch_accessed_variables == Py_True);
  Py_INCREF(tape);
  tape->nesting_id = tape_nesting_id_counter.fetch_add(1);
  GetTapeSet()->insert(tape);
  return reinterpret_cast<PyObject*>(tape);
}