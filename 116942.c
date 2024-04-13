  int64_t WatchVariable(PyObject* v) {
    tensorflow::Safe_PyObjectPtr handle(PyObject_GetAttrString(v, "handle"));
    if (handle == nullptr) {
      return -1;
    }
    int64_t id = FastTensorId(handle.get());

    tensorflow::mutex_lock l(watched_variables_mu_);
    auto insert_result = watched_variables_.emplace(id, v);

    if (insert_result.second) {
      // Only increment the reference count if we aren't already watching this
      // variable.
      Py_INCREF(v);
    }

    return id;
  }