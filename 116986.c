  ~VariableWatcher() {
    for (const IdAndVariable& v : watched_variables_) {
      Py_DECREF(v.variable);
    }
  }