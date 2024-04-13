  explicit RaggedTensorFromVariantOp(OpKernelConstruction* context)
      : OpKernel(context) {
    OP_REQUIRES_OK(context, context->GetAttr("input_ragged_rank",
                                             &input_ragged_rank_attr_));
    OP_REQUIRES_OK(
        context, context->GetAttr("output_ragged_rank", &output_ragged_rank_));
  }