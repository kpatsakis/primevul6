  bool IsDrivenByControlDependency(const NodeDef& node) const {
    return std::any_of(
        node.input().begin(), node.input().end(),
        [](const string& input) { return IsControlInput(input); });
  }