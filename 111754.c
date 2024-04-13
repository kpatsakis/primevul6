Status InferenceContext::MakeShapeFromPartialTensorShape(
    const PartialTensorShape& partial_shape, ShapeHandle* out) {
  *out = nullptr;
  if (partial_shape.dims() == -1) {
    return ReturnUnknownShape(out);
  }
  const int num_dims = partial_shape.dims();
  std::vector<DimensionHandle> dims(num_dims);
  for (int i = 0; i < num_dims; ++i) {
    // -1 is unknown in PartialTensorShape and in InferenceContext, so this size
    // can be passed directly to MakeDim.
    dims[i] = MakeDim(partial_shape.dim_size(i));
  }
  return ReturnCreatedShape(dims, out);
}