ShapeHandle InferenceContext::Vector(DimensionOrConstant dim) {
  return MakeShape({dim});
}