  bool IsSupported(const NodeDef* node) const override {
    return IsPow(*node) &&
           ctx().graph_properties->HasOutputProperties(node->name()) &&
           ctx().graph_properties->HasInputProperties(node->name());
  }