  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    Tensor pow;
    if (!GetTensorFromConstNode(node->input(1), &pow)) return Status::OK();
    complex128 prev, curr;
    for (int i = 0; i < pow.NumElements(); ++i) {
      if (!GetElementUnexhaustive(pow, i, {pow.dtype()}, &curr)) {
        // input data type is not supported by Pow. Skip.
        return Status::OK();
      }
      if (i != 0 && curr != prev) {
        // pow has different values on different elements. Skip.
        return Status::OK();
      }
      prev = curr;
    }
    NodeDef *x, *y;
    TF_RETURN_IF_ERROR(GetInputNode(node->input(0), &x));
    TF_RETURN_IF_ERROR(GetInputNode(node->input(1), &y));
    const auto& value_props =
        ctx().graph_properties->GetInputProperties(node->name())[0];
    const TensorShapeProto& output_shape =
        ctx().graph_properties->GetOutputProperties(node->name())[0].shape();
    if (curr == complex128(2, 0)) {
      node->set_op("Square");
      node->set_input(1, AsControlDependency(y->name()));
      AddToOptimizationQueue(node);
      AddToOptimizationQueue(y);
    } else if (curr == complex128(3, 0)) {
      // TODO(courbet): Use 'Cube' when it's added to TF ops.
      if (NodeIsOnCpu(*node)) {
        // We create an inner square node: inner_square = square(x)
        const NodeScopeAndName scope_and_name =
            ParseNodeScopeAndName(node->name());
        const string inner_square_name =
            OptimizedNodeName(scope_and_name, "_inner");
        NodeDef* inner_square_node = ctx().node_map->GetNode(inner_square_name);
        if (inner_square_node == nullptr) {
          inner_square_node = AddCopyNode(inner_square_name, node);
          inner_square_node->set_op("Square");
          inner_square_node->mutable_input()->RemoveLast();
        }
        ctx().node_map->AddOutput(x->name(), inner_square_node->name());
        // We modify `node`: node = mul(x, inner_square);
        node->set_op("Mul");
        node->set_input(1, inner_square_node->name());
        node->add_input(AsControlDependency(y->name()));

        AddToOptimizationQueue(node);
        AddToOptimizationQueue(inner_square_node);
        AddToOptimizationQueue(y);
      }
    } else if (curr == complex128(1, 0) &&
               ShapesSymbolicallyEqual(value_props.shape(), output_shape)) {
      // Pow could be used to broadcast, so make sure the shapes of the two
      // arguments are identical before replacing Pow with Identity.
      node->set_op("Identity");
      node->set_input(1, AsControlDependency(y->name()));
      AddToOptimizationQueue(node);
      AddToOptimizationQueue(y);
    } else if (curr == complex128(0.5, 0)) {
      node->set_op("Sqrt");
      node->set_input(1, AsControlDependency(y->name()));
      AddToOptimizationQueue(node);
      AddToOptimizationQueue(y);
    } else if (curr == complex128(0, 0) &&
               ShapesSymbolicallyEqual(value_props.shape(), output_shape) &&
               PartialTensorShape(output_shape).IsFullyDefined()) {
      const auto dtype = node->attr().at("T").type();
      Tensor ones(dtype, output_shape);
      for (int i = 0; i < ones.NumElements(); ++i) {
        TF_RETURN_IF_ERROR(SetElementToOne(i, &ones));
      }
      node->set_op("Const");
      (*node->mutable_attr())["dtype"].set_type(dtype);
      node->mutable_attr()->erase("T");
      ones.AsProtoTensorContent(
          (*node->mutable_attr())["value"].mutable_tensor());
      node->set_input(0, AsControlDependency(x->name()));
      node->set_input(1, AsControlDependency(y->name()));
      AddToOptimizationQueue(node);
      AddToOptimizationQueue(x);
      AddToOptimizationQueue(y);
    } else if (curr == complex128(-0.5, 0)) {
      node->set_op("Rsqrt");
      node->set_input(1, AsControlDependency(y->name()));
      AddToOptimizationQueue(node);
      AddToOptimizationQueue(y);
    } else if (curr == complex128(-1, 0)) {
      node->set_op("Reciprocal");
      node->set_input(1, AsControlDependency(y->name()));
      AddToOptimizationQueue(node);
      AddToOptimizationQueue(y);
    }
    return Status::OK();
  }