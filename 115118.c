  void Compute(OpKernelContext* context) override {
    const Tensor* input_tensor;
    OP_REQUIRES_OK(context, context->input("input", &input_tensor));
    const DataType& dtype = input_tensor->dtype();

    Tensor* output_tensor = nullptr;
    OP_REQUIRES_OK(context,
                   context->allocate_output("output", input_tensor->shape(),
                                            &output_tensor));
    auto output_flat = output_tensor->flat<tstring>();

#define ENCODE_TYPE(type, T, enc_str)                                     \
  case (type): {                                                          \
    const auto& input_flat = input_tensor->flat<T>();                     \
    for (int i = 0; i < input_flat.size(); ++i) {                         \
      output_flat(i) = strings::Printf((enc_str.c_str()), input_flat(i)); \
    }                                                                     \
  } break

    switch (dtype) {
      ENCODE_TYPE(DT_INT32, int32, format_);
      ENCODE_TYPE(DT_INT64, int64, format_);
      ENCODE_TYPE(DT_FLOAT, float, format_);
      ENCODE_TYPE(DT_DOUBLE, double, format_);
      ENCODE_TYPE(DT_INT8, int8, format_);
      ENCODE_TYPE(DT_INT16, int16, format_);
      case (DT_BOOL): {
        const auto& input_flat = input_tensor->flat<bool>();
        for (int i = 0; i < input_flat.size(); ++i) {
          output_flat(i) = (input_flat(i)) ? "true" : "false";
        }
      } break;
      case (DT_COMPLEX64): {
        const auto& input_flat = input_tensor->flat<complex64>();
        for (int i = 0; i < input_flat.size(); ++i) {
          output_flat(i) = strings::Printf(
              format_.c_str(), input_flat(i).real(), input_flat(i).imag());
        }
      } break;
      case (DT_COMPLEX128): {
        const auto& input_flat = input_tensor->flat<complex128>();
        for (int i = 0; i < input_flat.size(); ++i) {
          output_flat(i) = strings::Printf(
              format_.c_str(), input_flat(i).real(), input_flat(i).imag());
        }
      } break;
      default:
        bool can_encode_type = false;
        OP_REQUIRES(context, can_encode_type,
                    errors::InvalidArgument("Cannot encode input of type ",
                                            DataTypeString(dtype)));
    }

#undef ENCODE_TYPE
  }