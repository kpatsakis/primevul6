  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    NodeDef* input;
    TF_RETURN_IF_ERROR(GetInputNode(node->input(0), &input));
    if (input->op() == node->op() && input->device() == node->device()) {
      *simplified_node_name = node->input(0);
    }
    return Status::OK();
  }