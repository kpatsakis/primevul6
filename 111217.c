  void Compute(OpKernelContext* ctx) override {
    const Tensor& in0 = ctx->input(0);
    const Tensor& in1 = ctx->input(1);
    OP_REQUIRES(
        ctx, in0.NumElements() == in1.NumElements(),
        errors::InvalidArgument("The two arguments to a cwise op must have "
                                "same number of elements, got ",
                                in0.NumElements(), " and ", in1.NumElements()));
    auto in0_flat = in0.flat<Tin>();
    auto in1_flat = in1.flat<Tin>();
    const Device& eigen_device = ctx->eigen_device<Device>();

    Tensor* out = nullptr;
    if (std::is_same<Tin, Tout>::value) {
      OP_REQUIRES_OK(ctx, ctx->forward_input_or_allocate_output(
                              {0, 1}, 0, in0.shape(), &out));
    } else {
      OP_REQUIRES_OK(ctx, ctx->allocate_output(0, in0.shape(), &out));
    }
    auto out_flat = out->flat<Tout>();
    functor::SimpleBinaryFunctor<Device, Functor>()(eigen_device, out_flat,
                                                    in0_flat, in1_flat);
  }