  tensorflow::Status Initialize() {
    num_elements_ = PyObject_GetAttrString(py_vspace_, "num_elements_fn");
    if (num_elements_ == nullptr) {
      return tensorflow::errors::InvalidArgument("invalid vspace");
    }
    aggregate_fn_ = PyObject_GetAttrString(py_vspace_, "aggregate_fn");
    if (aggregate_fn_ == nullptr) {
      return tensorflow::errors::InvalidArgument("invalid vspace");
    }
    zeros_fn_ = PyObject_GetAttrString(py_vspace_, "zeros_fn");
    if (zeros_fn_ == nullptr) {
      return tensorflow::errors::InvalidArgument("invalid vspace");
    }
    zeros_like_fn_ = PyObject_GetAttrString(py_vspace_, "zeros_like_fn");
    if (zeros_like_fn_ == nullptr) {
      return tensorflow::errors::InvalidArgument("invalid vspace");
    }
    ones_fn_ = PyObject_GetAttrString(py_vspace_, "ones_fn");
    if (ones_fn_ == nullptr) {
      return tensorflow::errors::InvalidArgument("invalid vspace");
    }
    ones_like_fn_ = PyObject_GetAttrString(py_vspace_, "ones_like_fn");
    if (ones_like_fn_ == nullptr) {
      return tensorflow::errors::InvalidArgument("invalid vspace");
    }
    graph_shape_fn_ = PyObject_GetAttrString(py_vspace_, "graph_shape_fn");
    if (graph_shape_fn_ == nullptr) {
      return tensorflow::errors::InvalidArgument("invalid vspace");
    }
    return tensorflow::Status::OK();
  }