  void AddToOptimizationQueue(NodeDef* node) {
    ctx_ext_.nodes_to_simplify->PushBack(node);
  }