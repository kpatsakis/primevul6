inline bool Mean(const T* input_data, const int* input_dims,
                 const int input_num_dims, T* output_data,
                 const int* output_dims, const int output_num_dims,
                 const int* axis, const int num_axis_dimensions, bool keep_dims,
                 int* temp_index, int* resolved_axis, U* temp_sum) {
  ruy::profiler::ScopeLabel label("Mean");
  // Reset output data.
  size_t num_outputs = 1;
  for (int idx = 0; idx < output_num_dims; ++idx) {
    size_t current = static_cast<size_t>(output_dims[idx]);
    // Overflow prevention.
    if (num_outputs > std::numeric_limits<size_t>::max() / current) {
      return false;
    }
    num_outputs *= current;
  }
  for (size_t idx = 0; idx < num_outputs; ++idx) {
    output_data[idx] = T();
    temp_sum[idx] = U();
  }

  // Resolve axis.
  int num_resolved_axis = 0;
  if (!ResolveAxis(input_num_dims, axis, num_axis_dimensions, resolved_axis,
                   &num_resolved_axis)) {
    return false;
  }

  if (!ReduceSumImpl<T, U>(input_data, input_dims, output_dims, input_num_dims,
                           output_num_dims, resolved_axis, num_resolved_axis,
                           temp_index, temp_sum)) {
    return false;
  }

  // Calculate mean by dividing output_data by num of aggregated element.
  size_t num_elements_in_axis = 1;
  for (int idx = 0; idx < num_resolved_axis; ++idx) {
    size_t current = static_cast<size_t>(input_dims[resolved_axis[idx]]);
    // Overflow prevention.
    if (current > (std::numeric_limits<size_t>::max() / num_elements_in_axis)) {
      return false;
    }
    num_elements_in_axis *= current;
  }

  if (num_elements_in_axis > 0) {
    for (size_t idx = 0; idx < num_outputs; ++idx) {
      output_data[idx] =
          static_cast<T>(temp_sum[idx] / static_cast<U>(num_elements_in_axis));
    }
  }
  return true;
}