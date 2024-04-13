  bool IsScalar(mlir::Value value) const {
    auto type = value.getType().dyn_cast<ShapedType>();
    if (!type) {
      return false;
    }
    if (!type.hasStaticShape()) {
      return false;
    }
    return type.getNumElements() == 1;
  }