int MaybeRaiseExceptionFromTFStatus(TF_Status* status, PyObject* exception) {
  if (status->status.ok()) return 0;
  const char* msg = TF_Message(status);
  if (exception == nullptr) {
    tensorflow::mutex_lock l(exception_class_mutex);
    if (exception_class != nullptr) {
      tensorflow::Safe_PyObjectPtr payloads(PyDict_New());
      for (const auto& payload :
           tensorflow::errors::GetPayloads(status->status)) {
        PyDict_SetItem(payloads.get(),
                       PyBytes_FromString(payload.first.c_str()),
                       PyBytes_FromString(payload.second.c_str()));
      }
      tensorflow::Safe_PyObjectPtr val(Py_BuildValue(
          "siO", FormatErrorStatusStackTrace(status->status).c_str(),
          TF_GetCode(status), payloads.get()));
      if (PyErr_Occurred()) {
        // NOTE: This hides the actual error (i.e. the reason `status` was not
        // TF_OK), but there is nothing we can do at this point since we can't
        // generate a reasonable error from the status.
        // Consider adding a message explaining this.
        return -1;
      }
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