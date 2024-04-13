  bool IsSupported(const NodeDef* node) const override {
    if (!IsSub(*node)) return false;

    NodeDef* input;
    if (!GetInputNode(node->input(0), &input).ok()) return false;

    return IsExp(*input);
  }