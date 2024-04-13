inline bool ReduceGeneric(const T* input_data, const int* input_dims,
                          const int input_num_dims, T* output_data,
                          const int* output_dims, const int output_num_dims,
                          const int* axis, const int64_t num_axis_dimensions,
                          bool keep_dims, int* temp_index, int* resolved_axis,
                          T init_value,
                          T reducer(const T current, const T in)) {
  // Reset output data.
  if (!InitTensorDataForReduce(output_dims, output_num_dims, init_value,
                               output_data)) {
    return false;
  }

  // Resolve axis.
  int num_resolved_axis = 0;
  if (!ResolveAxis(input_num_dims, axis, num_axis_dimensions, resolved_axis,
                   &num_resolved_axis)) {
    return false;
  }

  return Reduce<T, T>(input_data, input_dims, output_dims, input_num_dims,
                      output_num_dims, resolved_axis, num_resolved_axis,
                      temp_index, reducer, output_data);
}