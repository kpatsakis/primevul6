static int64_t FastTensorId(PyObject* tensor) {
  if (EagerTensor_CheckExact(tensor)) {
    return PyEagerTensor_ID(tensor);
  }
  PyObject* id_field = PyObject_GetAttrString(tensor, "_id");
  if (id_field == nullptr) {
    return -1;
  }
  int64_t id = MakeInt(id_field);
  Py_DECREF(id_field);
  return id;
}