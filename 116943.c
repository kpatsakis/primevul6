static void TFE_Py_ForwardAccumulatorDelete(PyObject* accumulator) {
  delete reinterpret_cast<TFE_Py_ForwardAccumulator*>(accumulator)->accumulator;
  Py_TYPE(accumulator)->tp_free(accumulator);
}