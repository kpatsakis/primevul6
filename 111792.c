Status InferenceContext::MakeShapeFromTensor(const Tensor* t,
                                             ShapeHandle tensor_shape,
                                             ShapeHandle* out) {
  return InternalMakeShapeFromTensor(
      false /* treat_unknown_scalar_tensor_as_unknown_shape */, t, tensor_shape,
      out);
}