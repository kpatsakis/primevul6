  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    NodeDef* y;
    TF_RETURN_IF_ERROR(GetInputNode(node->input(1), &y));
    // Optimize only if divisor is a Sqrt whose output is not being consumed
    // elsewhere.
    if (IsSqrt(*y) && !IsInPreserveSet(*y) &&
        (NumNonControlOutputs(*y, *ctx().node_map) == 1)) {
      if (IsXdivy(*node)) {
        // xdivy(a, sqrt(b)) => mul_no_nan(rsqrt(b), a)
        node->set_op("MulNoNan");
        node->mutable_input()->SwapElements(0, 1);
      } else {
        // div(a, sqrt(b)) => mul(a, rsqrt(b))
        node->set_op("Mul");
      }
      y->set_op("Rsqrt");
      AddToOptimizationQueue(node);
      AddToOptimizationQueue(y);
    }
    return Status::OK();
  }