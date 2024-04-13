  bool IsOnes(const NodeDef& node) const {
    if (!IsReallyConstant(node)) return false;
    if (node.attr().at("dtype").type() != DT_FLOAT) return false;

    Tensor tensor;
    if (!tensor.FromProto(node.attr().at("value").tensor())) {
      return false;
    }

    auto values = tensor.flat<float>();
    for (int i = 0; i < tensor.NumElements(); ++i) {
      if (values(i) != 1.0f) {
        return false;
      }
    }

    return true;
  }