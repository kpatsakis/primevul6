Status InferenceContext::MakeShapeFromShapeTensor(int input_idx,
                                                  ShapeHandle* out) {
  ShapeHandle input_shape;
  TF_RETURN_IF_ERROR(WithRank(input(input_idx), 1, &input_shape));

  request_input_tensor_as_partial_shape(input_idx);
  const int input_tensors_as_shapes_size = input_tensors_as_shapes_.size();
  if (input_idx < input_tensors_as_shapes_size &&
      input_tensors_as_shapes_[input_idx].IsSet() &&
      RankKnown(input_tensors_as_shapes_[input_idx])) {
    *out = input_tensors_as_shapes_[input_idx];
    return Status::OK();
  }

  return InternalMakeShapeFromTensor(
      false /* treat_unknown_scalar_tensor_as_unknown_shape */,
      input_tensor(input_idx), input_shape, out);
}