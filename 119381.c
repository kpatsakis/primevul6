  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    NodeDef* x;
    TF_RETURN_IF_ERROR(GetInputNode(node->input(0), &x));
    // Optimize only if arg is a Softmax whose output is not being consumed
    // elsewhere.
    if (IsSoftmax(*x) && !IsInPreserveSet(*x) &&
        (NumNonControlOutputs(*x, *ctx().node_map) == 1)) {
      // Log(Softmax(x)) => LogSoftmax(Identity(x))
      node->set_op("LogSoftmax");
      x->set_op("Identity");
      AddToOptimizationQueue(node);
      AddToOptimizationQueue(x);
    }
    return Status::OK();
  }