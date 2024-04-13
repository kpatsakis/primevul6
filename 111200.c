  explicit ApproximateEqualOp(OpKernelConstruction* context)
      : OpKernel(context) {
    float tolerance;
    OP_REQUIRES_OK(context, context->GetAttr("tolerance", &tolerance));
    tolerance_ = T(tolerance);
  }