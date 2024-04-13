  bool IsSameOp(const OptimizedNodesGroup& group, const NodeDef& node) const {
    return group.root_node->op() == node.op();
  }