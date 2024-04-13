bool ParseInt64Value(const string& key, PyObject* py_value, TF_Status* status,
                     int64_t* value) {
  if (PyInt_Check(py_value)) {
    *value = static_cast<int64_t>(PyInt_AsLong(py_value));
    return true;
  } else if (PyLong_Check(py_value)) {
    *value = static_cast<int64_t>(PyLong_AsLong(py_value));
    return true;
  }
  TF_SetStatus(
      status, TF_INVALID_ARGUMENT,
      tensorflow::strings::StrCat("Expecting int or long value for attr ", key,
                                  ", got ", py_value->ob_type->tp_name)
          .c_str());
  return false;
}