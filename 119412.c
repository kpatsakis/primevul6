  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    // 1. Discard aggregate nodes with a single input and no control deps.
    if (node->input_size() == 1) {
      *simplified_node_name = node->input(0);
      return Status::OK();
    }

    // 2. Rewrite aggregations of N >= 2 identical terms.

    // All non-control inputs must be identical.
    bool all_equal = true;
    int num_inputs = 1;
    for (int i = 1; i < node->input_size(); ++i) {
      if (IsControlInput(node->input(i))) break;
      ++num_inputs;
      if (node->input(i) != node->input(0)) {
        all_equal = false;
        break;
      }
    }
    if (!all_equal) return Status::OK();

    // And node should not be optimized earlier.
    const NodeScopeAndName node_scope_and_name =
        ParseNodeScopeAndName(node->name());
    const string optimized_const_name =
        OptimizedNodeName(node_scope_and_name, "Const");
    const string optimized_mul_name =
        OptimizedNodeName(node_scope_and_name, "Mul");

    bool is_already_optimized =
        ctx().node_map->NodeExists(optimized_const_name) ||
        ctx().node_map->NodeExists(optimized_mul_name);

    if (is_already_optimized) return Status::OK();

    // At this point all preconditions are met, and we safely do the rewrite.
    VLOG(3) << "Simplify aggregation with identical inputs: node="
            << node->name() << " num_inputs=" << num_inputs;

    // 1. Create constant node with value N.
    const auto type = GetDataTypeFromAttr(*node, "T");
    Tensor t(type, TensorShape({}));
    Status status = SetTensorValue(type, num_inputs, &t);
    if (!status.ok()) {
      return errors::Internal("Failed to create const node: ",
                              status.error_message());
    }

    TensorValue value(&t);
    NodeDef* new_const_node = AddEmptyNode(optimized_const_name);
    status = ConstantFolding::CreateNodeDef(new_const_node->name(), value,
                                            new_const_node);
    if (!status.ok()) {
      return errors::Internal("Failed to create const node: ",
                              status.error_message());
    }
    new_const_node->set_device(node->device());
    MaybeAddControlInput(NodeName(node->input(0)), new_const_node,
                         ctx().optimized_graph, ctx().node_map);
    AddToOptimizationQueue(new_const_node);

    // 2. Replace the aggregate node with Mul(Const(N), x).
    NodeDef* new_mul_node = AddEmptyNode(optimized_mul_name);
    new_mul_node->set_op("Mul");
    new_mul_node->set_device(node->device());
    SetDataTypeToAttr(type, "T", new_mul_node);
    new_mul_node->add_input(new_const_node->name());
    ctx().node_map->AddOutput(new_const_node->name(), new_mul_node->name());
    new_mul_node->add_input(node->input(0));
    ctx().node_map->AddOutput(node->input(0), new_mul_node->name());

    ForwardControlDependencies(new_mul_node, {node});
    *simplified_node_name = new_mul_node->name();

    return Status::OK();
  }