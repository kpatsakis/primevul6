  bool IsReallyConstant(const NodeDef& node) const {
    if (!IsConstant(node)) {
      return false;
    }
    // If the node is fed it's not constant anymore.
    return ctx().feed_nodes->find(node.name()) == ctx().feed_nodes->end();
  }