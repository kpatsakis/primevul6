void SetOpAttrs(TFE_Context* ctx, TFE_Op* op, PyObject* attrs, int start_index,
                TF_Status* out_status) {
  if (attrs == Py_None) return;
  Py_ssize_t len = PyTuple_GET_SIZE(attrs) - start_index;
  if ((len & 1) != 0) {
    TF_SetStatus(out_status, TF_INVALID_ARGUMENT,
                 "Expecting attrs tuple to have even length.");
    return;
  }
  // Parse attrs
  for (Py_ssize_t i = 0; i < len; i += 2) {
    PyObject* py_key = PyTuple_GET_ITEM(attrs, start_index + i);
    PyObject* py_value = PyTuple_GET_ITEM(attrs, start_index + i + 1);
#if PY_MAJOR_VERSION >= 3
    const char* key = PyBytes_Check(py_key) ? PyBytes_AsString(py_key)
                                            : PyUnicode_AsUTF8(py_key);
#else
    const char* key = PyBytes_AsString(py_key);
#endif
    unsigned char is_list = 0;
    const TF_AttrType type = TFE_OpGetAttrType(op, key, &is_list, out_status);
    if (!out_status->status.ok()) return;
    if (is_list != 0) {
      if (!SetOpAttrList(ctx, op, key, py_value, type, nullptr, out_status))
        return;
    } else {
      if (!SetOpAttrScalar(ctx, op, key, py_value, type, nullptr, out_status))
        return;
    }
  }
}