static bool ShapeMatchesReduceWithKeepAxes(Value input,
                                           const mlir::Attribute &axes,
                                           const mlir::Attribute &shape) {
  RankedTensorType type = input.getType().dyn_cast_or_null<RankedTensorType>();
  if (!type) return false;

  DenseIntElementsAttr axes_attr =
      axes.dyn_cast_or_null<DenseIntElementsAttr>();
  DenseIntElementsAttr shape_attr =
      shape.dyn_cast_or_null<DenseIntElementsAttr>();
  if (!axes_attr || !shape_attr) return false;

  if (shape_attr.getNumElements() != type.getRank()) return false;

  llvm::SmallSet<uint64_t, 4> axes_set;
  for (auto a : axes_attr.getIntValues()) {
    axes_set.insert(a.getZExtValue());
  }

  auto type_shape = type.getShape();
  for (uint64_t i = 0; i < type.getRank(); ++i) {
    if (axes_set.contains(i)) {
      if (shape_attr.getValue<APInt>({i}) != 1) return false;
    } else {
      if (shape_attr.getValue<APInt>({i}) != type_shape[i]) return false;
    }
  }
  return true;
}