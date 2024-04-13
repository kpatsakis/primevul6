  string OptimizedNodeName(const NodeDef& node) const {
    return strings::StrCat(node.name(), "/unary_ops_composition");
  }