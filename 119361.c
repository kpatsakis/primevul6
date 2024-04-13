  string OuterNodeName(const NodeDef* node, bool is_div) const {
    auto scope_and_name = ParseNodeScopeAndName(node->name());
    return is_div ? OptimizedNodeName(scope_and_name, "Div")
                  : OptimizedNodeName(scope_and_name, "Mul");
  }