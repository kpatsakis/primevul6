  void Compute(OpKernelContext* c) override {
    const TensorShape& tl_a_shape = c->input(0).shape();
    const TensorShape& tl_b_shape = c->input(1).shape();
    OP_REQUIRES(
        c, tl_a_shape == tl_b_shape,
        errors::InvalidArgument("Incompatible input TensorList tensor shapes: ",
                                tl_a_shape.DebugString(), " vs. ",
                                tl_b_shape.DebugString()));
    AllocatorAttributes attr;
    std::unique_ptr<Tensor> tl_alias = c->forward_input(
        0 /*input_index*/, 0 /*output_index*/, DT_VARIANT, tl_a_shape,
        DEVICE_MEMORY /* input is always on DEVICE_MEMORY */, attr);

    // tl_a may be aliased by tl_alias.
    const Tensor& tl_a = c->input(0);
    const Tensor& tl_b = c->input(1);

    Tensor* output = nullptr;
    bool ok_to_alias = tl_alias != nullptr;
    if (tl_alias && tl_alias->dtype() == DT_VARIANT &&
        tl_alias->NumElements() > 0) {
      auto tl_a_t = tl_alias->flat<Variant>();
      for (int64 i = 0; i < tl_alias->NumElements(); ++i) {
        TensorList* aliased = tl_a_t(i).get<TensorList>();
        if (aliased == nullptr || !aliased->RefCountIsOne()) {
          ok_to_alias = false;
          break;
        }
      }
      if (ok_to_alias) {
        c->set_output(0, *tl_alias);
        output = tl_alias.get();
      }
    }
    if (!ok_to_alias) {
      // Couldn't alias the entire Tensor.  We'll be conservative and not try
      // to alias individual batch entries.
      attr.set_on_host(true);
      OP_REQUIRES_OK(c, c->allocate_output(0, tl_a_shape, &output, attr));
    }

    auto output_t = output->flat<Variant>();
    auto tl_a_t = tl_a.flat<Variant>();
    auto tl_b_t = tl_b.flat<Variant>();

    for (int64 i = 0; i < tl_a.NumElements(); ++i) {
      const TensorList* l_a = tl_a_t(i).get<TensorList>();
      const TensorList* l_b = tl_b_t(i).get<TensorList>();
      OP_REQUIRES(
          c, l_a != nullptr,
          errors::InvalidArgument("input_a is not a TensorList at index ", i,
                                  ".  Saw: '", tl_a_t(i).DebugString(), "'"));
      OP_REQUIRES(
          c, l_b != nullptr,
          errors::InvalidArgument("input_b is not a TensorList at index ", i,
                                  ".  Saw: '", tl_b_t(i).DebugString(), "'"));
      OP_REQUIRES(c, l_a->element_dtype == element_dtype_,
                  errors::InvalidArgument(
                      "input_a[", i, "].dtype != element_dtype.  Saw: ",
                      DataTypeString(l_a->element_dtype), " vs. ",
                      DataTypeString(element_dtype_)));
      OP_REQUIRES(c, l_b->element_dtype == element_dtype_,
                  errors::InvalidArgument(
                      "input_b[", i, "].dtype != element_dtype.  Saw: ",
                      DataTypeString(l_b->element_dtype), " vs. ",
                      DataTypeString(element_dtype_)));
      OP_REQUIRES(c, l_a->element_shape.IsIdenticalTo(l_b->element_shape),
                  errors::InvalidArgument(
                      "input_a and input_b TensorList element shapes are not "
                      "identical at index ",
                      i, ".  Saw ", l_a->element_shape.DebugString(), " vs. ",
                      l_b->element_shape.DebugString()));
      if (ok_to_alias) {
        TensorList* out = output_t(i).get<TensorList>();
        std::copy(l_b->tensors().begin(), l_b->tensors().end(),
                  std::back_inserter(out->tensors()));
      } else {
        TensorList out = l_a->Copy();
        std::copy(l_b->tensors().begin(), l_b->tensors().end(),
                  std::back_inserter(out.tensors()));
        output_t(i) = std::move(out);
      }
    }
  }