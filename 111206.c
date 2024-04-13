  void Compute(OpKernelContext* ctx) override {
    const Tensor& inp = ctx->input(0);
    Tensor* out = nullptr;
    if (std::is_same<Tin, Tout>::value) {
      OP_REQUIRES_OK(ctx, ctx->forward_input_or_allocate_output(
                              {0}, 0, inp.shape(), &out));
    } else {
      OP_REQUIRES_OK(ctx, ctx->allocate_output(0, inp.shape(), &out));
    }
    functor::UnaryFunctor<Device, Functor>()(
        ctx->eigen_device<Device>(), out->flat<Tout>(), inp.flat<Tin>());
  }