void TFE_Py_TapeSetRemove(PyObject* tape) {
  auto* stack = GetTapeSet();
  if (stack != nullptr) {
    stack->erase(reinterpret_cast<TFE_Py_Tape*>(tape));
  }
  // We kept a reference to the tape in the set to ensure it wouldn't get
  // deleted under us; cleaning it up here.
  Py_DECREF(tape);
}