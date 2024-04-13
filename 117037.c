bool ParseStringValue(const string& key, PyObject* py_value, TF_Status* status,
                      tensorflow::StringPiece* value) {
  if (PyBytes_Check(py_value)) {
    Py_ssize_t size = 0;
    char* buf = nullptr;
    if (PyBytes_AsStringAndSize(py_value, &buf, &size) < 0) return false;
    *value = tensorflow::StringPiece(buf, size);
    return true;
  }
#if PY_MAJOR_VERSION >= 3
  if (PyUnicode_Check(py_value)) {
    Py_ssize_t size = 0;
    const char* buf = PyUnicode_AsUTF8AndSize(py_value, &size);
    if (buf == nullptr) return false;
    *value = tensorflow::StringPiece(buf, size);
    return true;
  }
#endif
  TF_SetStatus(
      status, TF_INVALID_ARGUMENT,
      tensorflow::strings::StrCat("Expecting a string value for attr ", key,
                                  ", got ", py_value->ob_type->tp_name)
          .c_str());
  return false;
}