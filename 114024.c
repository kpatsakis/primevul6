std::pair<INDICES_TYPE, INDICES_TYPE> GetStrides(
    const TensorShape& input_shape, const TensorShape& segment_id_shape) {
  int64_t small_stride = 1;
  int64_t big_stride = 1;
  for (auto i = 0; i < input_shape.dims(); i++) {
    if (i < segment_id_shape.dims()) {
      small_stride *= segment_id_shape.dim_size(i);
    } else {
      big_stride *= input_shape.dim_size(i);
    }
  }
  return std::make_pair(big_stride, small_stride);
}