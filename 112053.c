  void DequantizeSlice(const Device& d, OpKernelContext* ctx,
                       const ConstVec& input, float min_range, float max_range,
                       Vec output) {
    // TODO(pauldonnelly): Factor out the similar calculations in quantize,
    //   dequantize and quantize_and_dequantize ops.
    const float half_range =
        !std::is_signed<T>::value
            ? 0.0f
            : (static_cast<float>(std::numeric_limits<T>::max()) -
               std::numeric_limits<T>::min() + 1) /
                  2.0f;

    if (mode_ == QUANTIZE_MODE_MIN_COMBINED) {
      const float scale_factor =
          (max_range - min_range) /
          (static_cast<float>(std::numeric_limits<T>::max()) -
           std::numeric_limits<T>::min());

      output.device(d) =
          ((input.template cast<float>() + half_range) * scale_factor) +
          min_range;
    } else if (mode_ == QUANTIZE_MODE_SCALED) {
      const int min_output_value =
          std::numeric_limits<T>::min() + (narrow_range_ ? 1 : 0);
      const float scale_factor =
          std::numeric_limits<T>::min() == 0
              ? (max_range / std::numeric_limits<T>::max())
              : std::max(min_range / min_output_value,
                         max_range / std::numeric_limits<T>::max());
      output.device(d) = input.template cast<float>() * scale_factor;
    }
  }