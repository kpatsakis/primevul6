  tensorflow::Status CallBackwardFunction(
      const string& op_type, PyBackwardFunction* backward_function,
      const std::vector<int64_t>& unneeded_gradients,
      tensorflow::gtl::ArraySlice<PyObject*> output_gradients,
      absl::Span<PyObject*> result) const final {
    PyObject* grads = PyTuple_New(output_gradients.size());
    for (int i = 0; i < output_gradients.size(); ++i) {
      if (output_gradients[i] == nullptr) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(grads, i, Py_None);
      } else {
        PyTuple_SET_ITEM(grads, i,
                         reinterpret_cast<PyObject*>(output_gradients[i]));
      }
    }
    PyObject* py_result = (*backward_function)(grads, unneeded_gradients);
    Py_DECREF(grads);
    if (py_result == nullptr) {
      return tensorflow::errors::Internal("gradient function threw exceptions");
    }
    PyObject* seq =
        PySequence_Fast(py_result, "expected a sequence of gradients");
    if (seq == nullptr) {
      return tensorflow::errors::InvalidArgument(
          "gradient function did not return a list");
    }
    int len = PySequence_Fast_GET_SIZE(seq);
    if (len != result.size()) {
      return tensorflow::errors::Internal(
          "Recorded operation '", op_type,
          "' returned too few gradients. Expected ", result.size(),
          " but received ", len);
    }
    PyObject** seq_array = PySequence_Fast_ITEMS(seq);
    VLOG(1) << "Gradient length is " << len;
    for (int i = 0; i < len; ++i) {
      PyObject* item = seq_array[i];
      if (item == Py_None) {
        result[i] = nullptr;
      } else {
        Py_INCREF(item);
        result[i] = item;
      }
    }
    Py_DECREF(seq);
    Py_DECREF(py_result);
    return tensorflow::Status::OK();
  }