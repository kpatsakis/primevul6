ShapeHandle InferenceContext::Matrix(DimensionOrConstant dim1,
                                     DimensionOrConstant dim2) {
  return MakeShape({dim1, dim2});
}