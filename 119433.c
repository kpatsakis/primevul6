  string InnerAddNodeName(const NodeDef* node) const {
    auto scope_and_name = ParseNodeScopeAndName(node->name());
    return OptimizedNodeName(scope_and_name, "AddV2");
  }