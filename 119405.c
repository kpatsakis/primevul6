  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    NodeDef* input;
    TF_RETURN_IF_ERROR(GetInputNode(node->input(0), &input));
    if (!IsAdd(*input)) {
      return Status::OK();
    }

    if (ctx().graph_properties->GetInputProperties(input->name()).size() < 2) {
      return Status::OK();
    }

    bool modified = false;
    TF_RETURN_IF_ERROR(TrySimplifyInternal(node, input, 0, 1, &modified));
    if (!modified) {
      TF_RETURN_IF_ERROR(TrySimplifyInternal(node, input, 1, 0, &modified));
    }
    if (modified) {
      *simplified_node_name = node->name();
    }
    return Status::OK();
  }