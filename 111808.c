Status InferenceContext::MakeShapeFromTensorShape(const TensorShape& shape,
                                                  ShapeHandle* out) {
  return MakeShapeFromPartialTensorShape(PartialTensorShape(shape.dim_sizes()),
                                         out);
}