int MaybeRaiseExceptionFromStatus(const tensorflow::Status& status,
                                  PyObject* exception) {
  if (status.ok()) return 0;
  const char* msg = status.error_message().c_str();
  if (exception == nullptr) {
    tensorflow::mutex_lock l(exception_class_mutex);
    if (exception_class != nullptr) {
      tensorflow::Safe_PyObjectPtr payloads(PyDict_New());
      for (const auto& element : tensorflow::errors::GetPayloads(status)) {
        PyDict_SetItem(payloads.get(),
                       PyBytes_FromString(element.first.c_str()),
                       PyBytes_FromString(element.second.c_str()));
      }
      tensorflow::Safe_PyObjectPtr val(
          Py_BuildValue("siO", FormatErrorStatusStackTrace(status).c_str(),
                        status.code(), payloads.get()));
      PyErr_SetObject(exception_class, val.get());
      return -1;
    } else {
      exception = PyExc_RuntimeError;
    }
  }
  // May be update already set exception.
  PyErr_SetString(exception, msg);
  return -1;
}