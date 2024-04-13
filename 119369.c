  void MarkAllMembersWithTag(const OptimizedNodesGroup& group,
                             const StringPiece tag) const {
    AddNodeAttr(tag, true, group.root_node);
    for (NodeDef* optimized_node : group.optimized_nodes) {
      AddNodeAttr(tag, true, optimized_node);
    }
  }