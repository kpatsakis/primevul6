  PyObject* GetVariablesAsPyTuple() {
    tensorflow::mutex_lock l(watched_variables_mu_);
    PyObject* result = PyTuple_New(watched_variables_.size());
    Py_ssize_t pos = 0;
    for (const IdAndVariable& id_and_variable : watched_variables_) {
      PyTuple_SET_ITEM(result, pos++, id_and_variable.variable);
      Py_INCREF(id_and_variable.variable);
    }
    return result;
  }