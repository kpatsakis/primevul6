  bool IsInPreserveSet(const NodeDef& node) const {
    return ctx().nodes_to_preserve->find(node.name()) !=
           ctx().nodes_to_preserve->end();
  }