  bool IsScalarizableSplatConstant(mlir::Value value,
                                   DenseElementsAttr *elements_attr) const {
    if (!matchPattern(value, m_Constant(elements_attr))) {
      return false;
    }
    auto element_type = value.getType().cast<ShapedType>().getElementType();
    // Ignore per-axis quantized constants because after converting to scalar,
    // we will lose per-axis qantization parameter.
    if (element_type.isa<quant::UniformQuantizedPerAxisType>()) {
      return false;
    }
    if (IsScalar(value)) {
      return false;
    }
    return elements_attr->isSplat();
  }