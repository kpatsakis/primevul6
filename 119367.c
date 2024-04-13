  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    NodeDef* b;
    TF_RETURN_IF_ERROR(GetInputNode(node->input(0), &b));
    // Optimize only if base is a Sub whose output is not being consumed
    // elsewhere.
    if (IsSub(*b) && !IsInPreserveSet(*b) &&
        (NumNonControlOutputs(*b, *ctx().node_map) == 1)) {
      // For complex, SquaredDiff computes conj(x-y)*(x-y), so this rewrite is
      // invalid.
      const DataType type = GetDataTypeFromAttr(*b, "T");
      if ((type == DT_COMPLEX64) || (type == DT_COMPLEX128))
        return Status::OK();
      node->set_op("Identity");
      b->set_op("SquaredDifference");
      AddToOptimizationQueue(node);
      AddToOptimizationQueue(b);
    }
    return Status::OK();
  }