void TFE_Py_TapeSetAdd(PyObject* tape) {
  Py_INCREF(tape);
  TFE_Py_Tape* tfe_tape = reinterpret_cast<TFE_Py_Tape*>(tape);
  if (!GetTapeSet()->insert(tfe_tape).second) {
    // Already exists in the tape set.
    Py_DECREF(tape);
  } else {
    tfe_tape->nesting_id = tape_nesting_id_counter.fetch_add(1);
  }
}