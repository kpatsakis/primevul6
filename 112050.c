  void DequantizeTensor(OpKernelContext* ctx, const Tensor& input,
                        const float min_range, const float max_range,
                        Tensor* output) {
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

      const auto& input_tensor = input.flat<T>();
      output->flat<float>() =
          ((input_tensor.template cast<float>() + half_range) * scale_factor) +
          min_range;

    } else if (mode_ == QUANTIZE_MODE_MIN_FIRST) {
      if (meta::IsSupportedAndEnabled() && std::is_same<T, quint8>()) {
        auto input_ui8_array = input.flat<quint8>();
        meta::Dequantize(ctx, input_ui8_array.data(), input_ui8_array.size(),
                         min_range, max_range, output->flat<float>().data());
      } else {
        QuantizedTensorToFloatInPlaceUsingEigen<T>(
            ctx->template eigen_device<Device>(), input, min_range, max_range,
            output);
      }
    } else if (mode_ == QUANTIZE_MODE_SCALED) {
      const int min_output_value =
          std::numeric_limits<T>::min() + (narrow_range_ ? 1 : 0);
      const float scale_factor =
          std::numeric_limits<T>::min() == 0
              ? (max_range / std::numeric_limits<T>::max())
              : std::max(min_range / min_output_value,
                         max_range / std::numeric_limits<T>::max());
      const auto& input_tensor = input.flat<T>();
      output->flat<float>() =
          input_tensor.template cast<int>().template cast<float>() *
          scale_factor;
    }
  }