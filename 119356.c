  bool GetTensorFromConstNode(const string& node_name_or_input,
                              Tensor* tensor) {
    const NodeDef* node = ctx().node_map->GetNode(node_name_or_input);
    return node != nullptr && IsReallyConstant(*node) &&
           CheckAttrExists(*node, "value").ok() &&
           tensor->FromProto(node->attr().at("value").tensor());
  }