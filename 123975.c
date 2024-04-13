inline bool QuantizedMeanOrSum(const T* input_data, int32_t input_zero_point,
                               float input_scale, const int* input_dims,
                               const int input_num_dims, T* output_data,
                               int32_t output_zero_point, float output_scale,
                               const int* output_dims,
                               const int output_num_dims, const int* axis,
                               const int num_axis_dimensions, bool keep_dims,
                               int* temp_index, int* resolved_axis, U* temp_sum,
                               bool compute_sum) {
  const bool uint8_case = std::is_same<T, uint8_t>::value;
  const bool int16_case = std::is_same<T, int16_t>::value;
  if (uint8_case) {
    ruy::profiler::ScopeLabel label(compute_sum ? "Sum/Uint8" : "Mean/Uint8");
  } else if (int16_case) {
    ruy::profiler::ScopeLabel label(compute_sum ? "Sum/Int16" : "Mean/Int16");
  } else {
    ruy::profiler::ScopeLabel label(compute_sum ? "Sum/Int8" : "Mean/Int8");
  }
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
    const float scale = input_scale / output_scale;
    if (compute_sum) {
      // TODO(b/116341117): Eliminate float and do this completely in 8bit.
      const float bias =
          -input_zero_point * scale * num_elements_in_axis + 0.5f;
      for (size_t idx = 0; idx < num_outputs; ++idx) {
        const U value =
            static_cast<U>(TfLiteRound(temp_sum[idx] * scale + bias)) +
            output_zero_point;
        output_data[idx] = static_cast<T>(value);
      }
    } else {
      const float bias = -input_zero_point * scale + 0.5f;
      for (size_t idx = 0; idx < num_outputs; ++idx) {
        float float_mean = static_cast<float>(temp_sum[idx]) /
                           static_cast<float>(num_elements_in_axis);
        float result = TfLiteMin(
            TfLiteRound(float_mean * scale + bias) + output_zero_point,
            static_cast<float>(std::numeric_limits<T>::max()));
        result = TfLiteMax(result,
                           static_cast<float>(std::numeric_limits<T>::min()));
        output_data[idx] = static_cast<T>(result);
      }
    }
  }
  return true;
}