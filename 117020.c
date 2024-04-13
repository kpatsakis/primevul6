void SetOpAttrListDefault(
    TFE_Context* ctx, TFE_Op* op, const tensorflow::OpDef::AttrDef& attr,
    const char* key, TF_AttrType type,
    tensorflow::gtl::FlatMap<string, int64_t>* attr_list_sizes,
    TF_Status* status) {
  if (type == TF_ATTR_STRING) {
    int num_values = attr.default_value().list().s_size();
    std::unique_ptr<const void*[]> values(new const void*[num_values]);
    std::unique_ptr<size_t[]> lengths(new size_t[num_values]);
    (*attr_list_sizes)[key] = num_values;
    for (int i = 0; i < num_values; i++) {
      const string& v = attr.default_value().list().s(i);
      values[i] = v.data();
      lengths[i] = v.size();
    }
    TFE_OpSetAttrStringList(op, key, values.get(), lengths.get(), num_values);
  } else if (type == TF_ATTR_INT) {
    int num_values = attr.default_value().list().i_size();
    std::unique_ptr<int64_t[]> values(new int64_t[num_values]);
    (*attr_list_sizes)[key] = num_values;
    for (int i = 0; i < num_values; i++) {
      values[i] = attr.default_value().list().i(i);
    }
    TFE_OpSetAttrIntList(op, key, values.get(), num_values);
  } else if (type == TF_ATTR_FLOAT) {
    int num_values = attr.default_value().list().f_size();
    std::unique_ptr<float[]> values(new float[num_values]);
    (*attr_list_sizes)[key] = num_values;
    for (int i = 0; i < num_values; i++) {
      values[i] = attr.default_value().list().f(i);
    }
    TFE_OpSetAttrFloatList(op, key, values.get(), num_values);
  } else if (type == TF_ATTR_BOOL) {
    int num_values = attr.default_value().list().b_size();
    std::unique_ptr<unsigned char[]> values(new unsigned char[num_values]);
    (*attr_list_sizes)[key] = num_values;
    for (int i = 0; i < num_values; i++) {
      values[i] = attr.default_value().list().b(i);
    }
    TFE_OpSetAttrBoolList(op, key, values.get(), num_values);
  } else if (type == TF_ATTR_TYPE) {
    int num_values = attr.default_value().list().type_size();
    std::unique_ptr<int[]> values(new int[num_values]);
    (*attr_list_sizes)[key] = num_values;
    for (int i = 0; i < num_values; i++) {
      values[i] = attr.default_value().list().type(i);
    }
    TFE_OpSetAttrTypeList(op, key,
                          reinterpret_cast<const TF_DataType*>(values.get()),
                          attr.default_value().list().type_size());
  } else if (type == TF_ATTR_SHAPE) {
    int num_values = attr.default_value().list().shape_size();
    (*attr_list_sizes)[key] = num_values;
    int total_dims = 0;
    for (int i = 0; i < num_values; ++i) {
      if (!attr.default_value().list().shape(i).unknown_rank()) {
        total_dims += attr.default_value().list().shape(i).dim_size();
      }
    }
    // Allocate a buffer that can fit all of the dims together.
    std::unique_ptr<int64_t[]> buffer(new int64_t[total_dims]);
    // Copy the input dims into the buffer and set dims to point to
    // the start of each list's dims.
    std::unique_ptr<const int64_t*[]> dims(new const int64_t*[num_values]);
    std::unique_ptr<int[]> num_dims(new int[num_values]);
    int64_t* offset = buffer.get();
    for (int i = 0; i < num_values; ++i) {
      const auto& shape = attr.default_value().list().shape(i);
      if (shape.unknown_rank()) {
        dims[i] = nullptr;
        num_dims[i] = -1;
      } else {
        for (int j = 0; j < shape.dim_size(); j++) {
          *offset = shape.dim(j).size();
          ++offset;
        }
      }
    }
    TFE_OpSetAttrShapeList(op, key, dims.get(), num_dims.get(), num_values,
                           status);
  } else if (type == TF_ATTR_FUNC) {
    int num_values = attr.default_value().list().func_size();
    (*attr_list_sizes)[key] = num_values;
    std::unique_ptr<const TFE_Op*[]> funcs(new const TFE_Op*[num_values]);
    for (int i = 0; i < num_values; i++) {
      funcs[i] = GetFunc(ctx, attr.default_value().list().func(i), status);
    }
    TFE_OpSetAttrFunctionList(op, key, funcs.get(), num_values);
  } else {
    TF_SetStatus(status, TF_UNIMPLEMENTED,
                 "Lists of tensors are not yet implemented for default valued "
                 "attributes for an operation.");
  }
}