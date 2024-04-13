  explicit GradientTape(bool persistent, bool watch_accessed_variables)
      : tensorflow::eager::GradientTape<PyObject, PyBackwardFunction,
                                        PyTapeTensor>(persistent),
        watch_accessed_variables_(watch_accessed_variables) {}