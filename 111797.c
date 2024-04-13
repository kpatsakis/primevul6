ShapeHandle InferenceContext::MakeShape(
    std::initializer_list<DimensionOrConstant> dims) {
  std::vector<DimensionHandle> dims_actual;
  dims_actual.reserve(dims.size());
  for (const DimensionOrConstant& d : dims) {
    dims_actual.push_back(MakeDim(d));
  }

  return shape_manager_.MakeShape(dims_actual);
}