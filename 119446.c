  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    NodeDef* x;
    NodeDef* y;
    TF_RETURN_IF_ERROR(GetInputNode(node->input(0), &x));
    TF_RETURN_IF_ERROR(GetInputNode(node->input(1), &y));
    bool updated = false;
    if (IsNeg(*y)) {
      // a - (-b) = a + b or  a + (-b) = a - b
      ForwardControlDependencies(node, {y});
      ctx().node_map->UpdateInput(node->name(), node->input(1), y->input(0));
      node->set_op(IsAdd(*node) ? "Sub" : "AddV2");
      node->set_input(1, y->input(0));
      updated = true;
    } else if (IsAdd(*node) && IsNeg(*x)) {
      // (-a) + b = b - a
      ForwardControlDependencies(node, {x});
      ctx().node_map->UpdateInput(node->name(), node->input(0), x->input(0));
      node->set_op("Sub");
      node->mutable_input()->SwapElements(0, 1);
      node->set_input(1, x->input(0));
      updated = true;
    }
    if (updated) {
      AddToOptimizationQueue(node);
    }
    return Status::OK();
  }