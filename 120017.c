  explicit DecodeBmpOp(OpKernelConstruction* context) : OpKernel(context) {
    OP_REQUIRES_OK(context, context->GetAttr("channels", &channels_));
    OP_REQUIRES(
        context,
        channels_ == 0 || channels_ == 1 || channels_ == 3 || channels_ == 4,
        errors::InvalidArgument("channels must be 0, 1, 3 or 4, got ",
                                channels_));
  }