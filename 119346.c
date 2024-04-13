  bool IsCastFromSupportedType(const NodeDef& node, DataType* out_input_type) {
    if (!IsCast(node)) return false;
    if (!GetNodeAttr(node, "SrcT", out_input_type).ok()) return false;
    return *out_input_type == DT_INT32 || *out_input_type == DT_INT64;
  }