  explicit MaxPooling3dGradGradOp(OpKernelConstruction* context)
      : OpKernel(context) {
    string data_format;
    OP_REQUIRES_OK(context, context->GetAttr("data_format", &data_format));
    OP_REQUIRES(context, FormatFromString(data_format, &data_format_),
                errors::InvalidArgument("Invalid data format"));
    OP_REQUIRES_OK(context, context->GetAttr("ksize", &ksize_));
    OP_REQUIRES(context, ksize_.size() == 5,
                errors::InvalidArgument("Sliding window ksize field must "
                                        "specify 5 dimensions"));
    OP_REQUIRES_OK(context, context->GetAttr("strides", &stride_));
    OP_REQUIRES(context, stride_.size() == 5,
                errors::InvalidArgument("Sliding window strides field must "
                                        "specify 5 dimensions"));
    OP_REQUIRES_OK(context, context->GetAttr("padding", &padding_));
    OP_REQUIRES(context, ksize_[0] == 1 && stride_[0] == 1,
                errors::Unimplemented(
                    "Pooling is not yet supported on the batch dimension."));
    const int32 ksize_c = GetTensorDim(ksize_, data_format_, 'C');
    const int32 stride_c = GetTensorDim(stride_, data_format_, 'C');
    OP_REQUIRES(context, ksize_c == 1 && stride_c == 1,
                errors::Unimplemented("MaxPooling3dGradGrad is not yet "
                                      "supported on the depth dimension."));
  }