  bool IsOnTheSameDevice(const OptimizedNodesGroup& group,
                         const NodeDef& node) const {
    return group.root_node->device() == node.device();
  }