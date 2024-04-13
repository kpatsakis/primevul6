  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    if (ctx().graph_properties->GetInputProperties(node->name()).size() < 2) {
      return Status::OK();
    }
    const auto& t = ctx().graph_properties->GetInputProperties(node->name())[0];
    const auto& c = ctx().graph_properties->GetInputProperties(node->name())[1];
    TensorShapeProto broadcast_shape;
    if (!ShapeAfterBroadcast(t.shape(), c.shape(), &broadcast_shape)) {
      return Status::OK();
    }
    if (!ShapesSymbolicallyEqual(t.shape(), broadcast_shape)) {
      // skip if the non-constant tensor doesn't have the same shape after
      // broadcast.
      return Status::OK();
    }
    Tensor constant;
    if (!GetTensorFromConstNode(node->input(1), &constant)) return Status::OK();
    // TODO(rmlarsen): Use the more general IsOnes helper here.
    complex128 element;
    for (int k = 0; k < constant.NumElements(); ++k) {
      if (!GetElementUnexhaustive(constant, k,
                                  {DT_BFLOAT16, DT_HALF, DT_FLOAT, DT_DOUBLE,
                                   DT_COMPLEX64, DT_COMPLEX128},
                                  &element)) {
        // input data type is not supported by expm1. Skip.
        return Status::OK();
      }
      if (element != complex128(1)) {
        // current element is not 1. Skip.
        return Status::OK();
      }
    }
    NodeDef* exp;
    TF_RETURN_IF_ERROR(GetInputNode(node->input(0), &exp));
    NodeDef *exp_input, *ones;
    TF_RETURN_IF_ERROR(GetInputNode(exp->input(0), &exp_input));
    TF_RETURN_IF_ERROR(GetInputNode(node->input(1), &ones));
    node->set_op("Expm1");
    node->set_input(0, exp->input(0));
    node->set_input(1, AsControlDependency(ones->name()));
    ForwardControlDependencies(node, {exp});

    AddToOptimizationQueue(node);
    AddToOptimizationQueue(exp);
    AddToOptimizationQueue(exp_input);
    AddToOptimizationQueue(ones);
    *simplified_node_name = node->name();
    return Status::OK();
  }