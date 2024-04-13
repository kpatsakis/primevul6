  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    const string node_name = node->name();
    NodeDef* input;
    TF_RETURN_IF_ERROR(GetInputNode(node->input(0), &input));
    if (IsInPreserveSet(*input) ||
        NumNonControlOutputs(*input, *ctx().node_map) > 1) {
      return Status::OK();
    }
    string new_op;
    if (IsEqual(*input)) {
      new_op = "NotEqual";
    } else if (IsNotEqual(*input)) {
      new_op = "Equal";
    } else if (IsLess(*input)) {
      new_op = "GreaterEqual";
    } else if (IsLessEqual(*input)) {
      new_op = "Greater";
    } else if (IsGreater(*input)) {
      new_op = "LessEqual";
    } else if (IsGreaterEqual(*input)) {
      new_op = "Less";
    }
    if (!new_op.empty()) {
      input->set_op(new_op);
      *simplified_node_name = input->name();
    }
    return Status::OK();
  }