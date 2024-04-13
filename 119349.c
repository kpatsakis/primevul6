  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    node_is_concat_ = IsConcat(*node);
    int prefix_length;
    std::set<string> ctrl_inputs;
    ChainLinkSet tails;
    TF_RETURN_IF_ERROR(
        FindCommonUnaryOpChain(*node, &prefix_length, &tails, &ctrl_inputs));
    if (prefix_length > 0 && !tails.empty()) {
      TF_RETURN_IF_ERROR(
          HoistUnaryOpChain(prefix_length, tails, &ctrl_inputs, node));
    }
    return Status::OK();
  }