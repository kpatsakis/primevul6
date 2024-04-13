  bool NodeIsAlreadyFused(const NodeDef& node) const {
    return fused_nodes_.count(node.name()) > 0;
  }