  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    // Check that we have a chain of (reshape -> unary -> reshape)
    NodeDef* reshape_b = node;
    if (!IsReshape(*reshape_b) || IsInPreserveSet(*reshape_b)) {
      return Status::OK();
    }

    NodeDef* unary;
    TF_RETURN_IF_ERROR(GetInputNode(reshape_b->input(0), &unary));
    if (!IsUnaryElementWise(*unary) || IsInPreserveSet(*unary)) {
      return Status::OK();
    }

    NodeDef* reshape_a;
    TF_RETURN_IF_ERROR(GetInputNode(unary->input(0), &reshape_a));
    if (!IsReshape(*reshape_a) || IsInPreserveSet(*reshape_a)) {
      return Status::OK();
    }

    NodeDef* input;
    TF_RETURN_IF_ERROR(GetInputNode(reshape_a->input(0), &input));

    const string new_reshape_name =
        OptimizedNodeName(ParseNodeScopeAndName(reshape_a->name()));
    if (ctx().node_map->NodeExists(new_reshape_name)) {
      return Status::OK();
    }

    // Attach unary to the input, bypassing reshape_a
    unary->set_input(0, reshape_a->input(0));
    ctx().node_map->UpdateInput(unary->name(), reshape_a->name(),
                                reshape_a->input(0));
    // Invalidate node properties since the shape of unary will be different
    ctx().graph_properties->ClearOutputProperties(unary->name());
    ctx().graph_properties->ClearInputProperties(unary->name());

    ForwardControlDependencies(unary, {reshape_a});

    // Create a copy of reshape_a and insert it after unary
    NodeDef* new_reshape =
        CopyReshapeAndInsertAfter(reshape_a, unary, new_reshape_name);
    AddToOptimizationQueue(new_reshape);

    *simplified_node_name = node->name();
    return Status::OK();
  }