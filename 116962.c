bool ParseBoolValue(const string& key, PyObject* py_value, TF_Status* status,
                    unsigned char* value) {
  if (PyBool_Check(py_value)) {
    *value = PyObject_IsTrue(py_value);
    return true;
  }
  TF_SetStatus(
      status, TF_INVALID_ARGUMENT,
      tensorflow::strings::StrCat("Expecting bool value for attr ", key,
                                  ", got ", py_value->ob_type->tp_name)
          .c_str());
  return false;
}