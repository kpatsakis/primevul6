  void Compute(OpKernelContext* ctx) override {
    const Tensor& max_num_elements_t = ctx->input(1);
    OP_REQUIRES(
        ctx, TensorShapeUtils::IsScalar(max_num_elements_t.shape()),
        errors::InvalidArgument(
            "max_num_elements expected to be a scalar ",
            "but got shape: ", max_num_elements_t.shape().DebugString()));
    Tensor* result;
    AllocatorAttributes attr;
    attr.set_on_host(true);
    OP_REQUIRES_OK(ctx, ctx->allocate_output(0, TensorShape{}, &result, attr));
    TensorList empty;
    empty.element_dtype = element_dtype_;
    empty.max_num_elements = max_num_elements_t.scalar<int32>()();
    PartialTensorShape element_shape;
    OP_REQUIRES_OK(ctx, TensorShapeFromTensor(ctx->input(0), &element_shape));
    empty.element_shape = element_shape;
    result->scalar<Variant>()() = std::move(empty);
  }