  explicit UnsortedSegmentJoinOp(OpKernelConstruction* ctx) : OpKernel(ctx) {
    OP_REQUIRES_OK(ctx, ctx->GetAttr("separator", &separator_));
  }