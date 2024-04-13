  void Compute(OpKernelContext* ctx) override {
    const Tensor& inp = ctx->input(0);
    OP_REQUIRES(
        ctx, TensorShapeUtils::IsScalar(inp.shape()),
        errors::InvalidArgument("Non-scalar variants are not supported."));
    const Variant& v = inp.scalar<Variant>()();
    Variant v_out;
    OP_REQUIRES_OK(ctx, UnaryOpVariant<Device>(ctx, OpEnum, v, &v_out));
    int numa_node = ctx->device()->NumaNode();
    Tensor out(cpu_allocator(numa_node), DT_VARIANT, TensorShape());
    out.scalar<Variant>()() = std::move(v_out);
    ctx->set_output(0, std::move(out));
  }