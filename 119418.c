void SetDataTypeToAttr(DataType dtype, const string& attr_name, NodeDef* node) {
  (*node->mutable_attr())[attr_name].set_type(dtype);
}