  void WatchVariable(PyObject* v) {
    int64_t id = variable_watcher_.WatchVariable(v);

    if (!PyErr_Occurred()) {
      this->Watch(id);
    }
  }