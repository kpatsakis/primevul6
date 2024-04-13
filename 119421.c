  bool IsAlreadyOptimized(const NodeDef& node) const {
    return optimized_nodes_.find(node.name()) != optimized_nodes_.end();
  }