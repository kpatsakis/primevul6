bool IsF32Value(Value value) {
  return value.getType().cast<ShapedType>().getElementType().isF32();
}