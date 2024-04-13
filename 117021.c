  void VariableAccessed(PyObject* v) {
    if (watch_accessed_variables_) {
      WatchVariable(v);
    }
  }