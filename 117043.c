bool ParseTypeValue(const string& key, PyObject* py_value, TF_Status* status,
                    int* value) {
  if (IsInteger(py_value)) {
    return ParseIntValue(key, py_value, status, value);
  }

  tensorflow::Safe_PyObjectPtr py_type_enum(
      PyObject_GetAttrString(py_value, "_type_enum"));
  if (py_type_enum == nullptr) {
    PyErr_Clear();
    TF_SetStatus(
        status, TF_INVALID_ARGUMENT,
        tensorflow::strings::StrCat("Expecting a DType.dtype for attr ", key,
                                    ", got ", py_value->ob_type->tp_name)
            .c_str());
    return false;
  }

  return ParseIntValue(key, py_type_enum.get(), status, value);
}