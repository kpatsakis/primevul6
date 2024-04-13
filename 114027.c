TensorShape GetOutputShape(const TensorShape& input_shape,
                           const TensorShape& segment_id_shape,
                           const int64_t num_segments) {
  TensorShape output_shape;
  output_shape.AddDim(num_segments);
  for (size_t index = segment_id_shape.dims(); index < input_shape.dims();
       ++index) {
    output_shape.AddDim(input_shape.dim_size(index));
  }
  return output_shape;
}