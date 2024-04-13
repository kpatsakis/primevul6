static void TFE_Py_VariableWatcher_Delete(PyObject* variable_watcher) {
  delete reinterpret_cast<TFE_Py_VariableWatcher*>(variable_watcher)
      ->variable_watcher;
  Py_TYPE(variable_watcher)->tp_free(variable_watcher);
}