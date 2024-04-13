  ~PyVSpace() override {
    Py_XDECREF(num_elements_);
    Py_XDECREF(aggregate_fn_);
    Py_XDECREF(zeros_fn_);
    Py_XDECREF(zeros_like_fn_);
    Py_XDECREF(ones_fn_);
    Py_XDECREF(ones_like_fn_);
    Py_XDECREF(graph_shape_fn_);

    Py_DECREF(py_vspace_);
  }