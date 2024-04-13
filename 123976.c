inline void Mean(const tflite::MeanParams& op_params,
                 const RuntimeShape& unextended_input_shape,
                 const uint8_t* input_data, int32_t input_zero_point,
                 float input_scale, const RuntimeShape& unextended_output_shape,
                 uint8_t* output_data, int32_t output_zero_point,
                 float output_scale) {
  ruy::profiler::ScopeLabel label("Mean4D/Uint8");

  // Current implementation only supports dimension equals 4 and simultaneous
  // reduction over width and height.
  TFLITE_CHECK_EQ(unextended_input_shape.DimensionsCount(), 4);
  TFLITE_CHECK_LE(unextended_output_shape.DimensionsCount(), 4);
  const RuntimeShape input_shape =
      RuntimeShape::ExtendedShape(4, unextended_input_shape);
  const RuntimeShape output_shape =
      RuntimeShape::ExtendedShape(4, unextended_output_shape);
  const int output_batch = output_shape.Dims(0);
  const int output_height = output_shape.Dims(1);
  const int output_width = output_shape.Dims(2);
  const int output_depth = output_shape.Dims(3);
  const int input_height = input_shape.Dims(1);
  const int input_width = input_shape.Dims(2);
  const float num_elements_in_axis = input_width * input_height;

  TFLITE_CHECK_EQ(op_params.axis_count, 2);
  TFLITE_CHECK((op_params.axis[0] == 1 && op_params.axis[1] == 2) ||
               (op_params.axis[0] == 2 && op_params.axis[1] == 1));
  TFLITE_CHECK_EQ(output_height, 1);
  TFLITE_CHECK_EQ(output_width, 1);

  constexpr int32_t kMinValue = std::numeric_limits<uint8_t>::min();
  constexpr int32_t kMaxValue = std::numeric_limits<uint8_t>::max();

  int32_t bias =
      output_zero_point -
      static_cast<int32_t>(input_zero_point * input_scale / output_scale);
  double real_scale =
      static_cast<double>(input_scale / (num_elements_in_axis * output_scale));

  int32_t multiplier;
  int shift;
  QuantizeMultiplier(real_scale, &multiplier, &shift);
  for (int out_b = 0; out_b < output_batch; ++out_b) {
    for (int out_d = 0; out_d < output_depth; ++out_d) {
      int32_t acc = 0;
      for (int in_h = 0; in_h < input_height; ++in_h) {
        for (int in_w = 0; in_w < input_width; ++in_w) {
          acc += input_data[Offset(input_shape, out_b, in_h, in_w, out_d)];
        }
      }
      acc = MultiplyByQuantizedMultiplier(acc, multiplier, shift);
      acc += bias;
      acc = std::min(std::max(acc, kMinValue), kMaxValue);
      output_data[Offset(output_shape, out_b, 0, 0, out_d)] =
          static_cast<uint8_t>(acc);
    }
  }
}