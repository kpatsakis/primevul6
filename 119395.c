  bool IsSupported(const NodeDef* node) const override {
    return IsAggregate(*node) && HasRegularInputs(*node) &&
           GetDataTypeFromAttr(*node, "T") !=
               DT_VARIANT;  // TODO(b/119787146): Enable for variants.
  }