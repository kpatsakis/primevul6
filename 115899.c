bool CanFuseConvOrDepthwiseConv(Attribute filter, Attribute val,
                                bool is_depthwise) {
  if (const auto elements = val.dyn_cast<DenseElementsAttr>()) {
    if (const auto filter_elements = filter.dyn_cast<DenseElementsAttr>()) {
      return CanFuseConvOrDepthwiseConvShapes(
          filter_elements.getType().getShape(), elements.getType().getShape(),
          is_depthwise);
    }
  }
  return false;
}