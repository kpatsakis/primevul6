Status InferenceContext::MakeShapeFromShapeProto(const TensorShapeProto& proto,
                                                 ShapeHandle* out) {
  *out = nullptr;
  TF_RETURN_IF_ERROR(PartialTensorShape::IsValidShape(proto));
  PartialTensorShape partial_shape(proto);
  return MakeShapeFromPartialTensorShape(partial_shape, out);
}