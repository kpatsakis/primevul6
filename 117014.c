void SetOpAttrScalarDefault(
    TFE_Context* ctx, TFE_Op* op, const tensorflow::AttrValue& default_value,
    const char* attr_name,
    tensorflow::gtl::FlatMap<string, int64_t>* attr_list_sizes,
    TF_Status* status) {
  SetOpAttrValueScalar(ctx, op, default_value, attr_name, status);
  if (default_value.value_case() == tensorflow::AttrValue::kI) {
    (*attr_list_sizes)[attr_name] = default_value.i();
  }
}