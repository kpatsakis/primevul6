  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    TF_RETURN_IF_ERROR(EnsureNodeIsSupported(node));

    // Bypass Cast whose source type and destination type are equal.
    AttrSlice attrs(*node);
    DataType input_type;
    TF_RETURN_IF_ERROR(GetNodeAttr(attrs, "SrcT", &input_type));
    DataType output_type;
    TF_RETURN_IF_ERROR(GetNodeAttr(attrs, "DstT", &output_type));
    if (input_type == output_type) {
      *simplified_node_name = node->input(0);
    }
    return Status::OK();
  }