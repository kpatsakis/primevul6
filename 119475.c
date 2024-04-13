  bool IsSupported(const NodeDef* node) const override {
    if (!CanOptimize(*node)) return false;

    // shape must be symbolically defined and all inputs compatible with it
    const OpInfo::TensorProperties* properties;
    Status has_properties = GetTensorProperties(node->name(), &properties);
    return has_properties.ok() && ShapeIsSymbolicallyDefined(*properties) &&
           HasAllInputsBroadcastableToShape(*node, *properties);
  }