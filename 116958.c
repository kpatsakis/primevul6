void SetOpAttrWithDefaults(
    TFE_Context* ctx, TFE_Op* op, const tensorflow::OpDef::AttrDef& attr,
    const char* attr_name, PyObject* attr_value,
    tensorflow::gtl::FlatMap<string, int64_t>* attr_list_sizes,
    TF_Status* status) {
  unsigned char is_list = 0;
  const TF_AttrType type = TFE_OpGetAttrType(op, attr_name, &is_list, status);
  if (!status->status.ok()) return;
  if (attr_value == Py_None) {
    if (is_list != 0) {
      SetOpAttrListDefault(ctx, op, attr, attr_name, type, attr_list_sizes,
                           status);
    } else {
      SetOpAttrScalarDefault(ctx, op, attr.default_value(), attr_name,
                             attr_list_sizes, status);
    }
  } else {
    if (is_list != 0) {
      SetOpAttrList(ctx, op, attr_name, attr_value, type, attr_list_sizes,
                    status);
    } else {
      SetOpAttrScalar(ctx, op, attr_name, attr_value, type, attr_list_sizes,
                      status);
    }
  }
}