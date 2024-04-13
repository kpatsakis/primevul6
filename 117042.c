bool ParseDimensionValue(const string& key, PyObject* py_value,
                         TF_Status* status, int64_t* value) {
  if (IsInteger(py_value)) {
    return ParseInt64Value(key, py_value, status, value);
  }

  tensorflow::Safe_PyObjectPtr dimension_value(
      PyObject_GetAttrString(py_value, "_value"));
  if (dimension_value == nullptr) {
    PyErr_Clear();
    TF_SetStatus(
        status, TF_INVALID_ARGUMENT,
        tensorflow::strings::StrCat("Expecting a Dimension for attr ", key,
                                    ", got ", py_value->ob_type->tp_name)
            .c_str());
    return false;
  }

  if (dimension_value.get() == Py_None) {
    *value = -1;
    return true;
  }

  return ParseInt64Value(key, dimension_value.get(), status, value);
}