  explicit RestoreV2(OpKernelConstruction* context) : OpKernel(context) {
    OP_REQUIRES_OK(context, context->GetAttr("dtypes", &dtypes_));
  }