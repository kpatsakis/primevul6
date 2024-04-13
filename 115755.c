  explicit FusedBatchNormOpBase(OpKernelConstruction* context,
                                bool is_batch_norm_ex = false)
      : OpKernel(context) {
    float epsilon;
    OP_REQUIRES_OK(context, context->GetAttr("epsilon", &epsilon));
    epsilon_ = U(epsilon);
    float exponential_avg_factor;
    OP_REQUIRES_OK(context, context->GetAttr("exponential_avg_factor",
                                             &exponential_avg_factor));
    exponential_avg_factor_ = U(exponential_avg_factor);
    string tensor_format;
    OP_REQUIRES_OK(context, context->GetAttr("data_format", &tensor_format));
    OP_REQUIRES(context, FormatFromString(tensor_format, &tensor_format_),
                errors::InvalidArgument("Invalid data format"));
    OP_REQUIRES_OK(context, context->GetAttr("is_training", &is_training_));

    if (!is_batch_norm_ex) {
      has_side_input_ = false;
      activation_mode_ = FbnActivationMode::kIdentity;
    } else {
      OP_REQUIRES_OK(context, ParseActivationMode(context, &activation_mode_));

      int num_side_inputs;
      OP_REQUIRES_OK(context,
                     context->GetAttr("num_side_inputs", &num_side_inputs));
      OP_REQUIRES(context, num_side_inputs >= 0 && num_side_inputs <= 1,
                  errors::InvalidArgument(
                      "FusedBatchNorm accepts at most one side input."));
      has_side_input_ = (num_side_inputs == 1);
      if (has_side_input_ && is_training_) {
        OP_REQUIRES(
            context, activation_mode_ != FbnActivationMode::kIdentity,
            errors::InvalidArgument("Identity activation is not supported with "
                                    "non-empty side input"));
      }
    }

    if (activation_mode_ != FbnActivationMode::kIdentity && is_training_) {
      // NOTE(ezhulenev): Following requirements are coming from implementation
      // details of cudnnBatchNormalizationForwardTrainingEx used in training
      // mode. In inference mode we call custom CUDA kernel that supports all
      // data formats and data types.
      OP_REQUIRES(context, DataTypeToEnum<T>::value == DT_HALF,
                  errors::InvalidArgument("FusedBatchNorm with activation "
                                          "supports only DT_HALF data type."));
      OP_REQUIRES(context, tensor_format_ == FORMAT_NHWC,
                  errors::InvalidArgument("FusedBatchNorm with activation "
                                          "supports only NHWC tensor format."));
      OP_REQUIRES(context, functor::BatchnormSpatialPersistentEnabled(),
                  errors::InvalidArgument(
                      "FusedBatchNorm with activation must run with cuDNN "
                      "spatial persistence mode enabled."));
    }
  }