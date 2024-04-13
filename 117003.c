DataType PyTensor_DataType(PyObject* tensor) {
  if (EagerTensor_CheckExact(tensor)) {
    return PyEagerTensor_Dtype(tensor);
  } else {
#if PY_MAJOR_VERSION < 3
    // Python 2.x:
    static PyObject* dtype_attr = PyString_InternFromString("dtype");
    static PyObject* type_enum_attr = PyString_InternFromString("_type_enum");
#else
    // Python 3.x:
    static PyObject* dtype_attr = PyUnicode_InternFromString("dtype");
    static PyObject* type_enum_attr = PyUnicode_InternFromString("_type_enum");
#endif
    Safe_PyObjectPtr dtype_field(PyObject_GetAttr(tensor, dtype_attr));
    if (!dtype_field) {
      return DT_INVALID;
    }

    Safe_PyObjectPtr enum_field(
        PyObject_GetAttr(dtype_field.get(), type_enum_attr));
    if (!enum_field) {
      return DT_INVALID;
    }

    return static_cast<DataType>(MakeInt(enum_field.get()));
  }
}