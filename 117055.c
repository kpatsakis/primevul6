  void Compute(OpKernelContext* context) override {
    // Read input Tensor.
    const Tensor& encoded_variant = context->input(0);
    auto input_ragged_rank_ = input_ragged_rank_attr_;

    if (input_ragged_rank_ == -1) {  // Infer input_ragged_rank_.
      input_ragged_rank_ = output_ragged_rank_ - encoded_variant.dims();
      OP_REQUIRES(context, input_ragged_rank_ >= 0,
                  errors::InvalidArgument(
                      "Inferred input_ragged_rank (output_ragged_rank - "
                      "encoded_variant.dims()) must be >= 0, found "
                      "output_ragged_rank: ",
                      output_ragged_rank_,
                      ", encoded_variant.dims(): ", encoded_variant.dims(),
                      ", inferred input_ragged_rank: ", input_ragged_rank_));
    }
    OP_REQUIRES(
        context,
        output_ragged_rank_ == encoded_variant.dims() + input_ragged_rank_,
        errors::InvalidArgument(
            "output_ragged_rank must be equal to input_ragged_rank + "
            "encoded_ragged.dims(); output_ragged_rank: ",
            output_ragged_rank_, ", input_ragged_rank: ", input_ragged_rank_,
            ", encoded_variant.dims(): ", encoded_variant.dims(), "."));

    // Decode all variants.
    const auto value_dtype = DataTypeToEnum<VALUE_TYPE>::v();
    const auto split_dtype = DataTypeToEnum<SPLIT_TYPE>::v();
    std::vector<RaggedTensorVariant> decoded_components;
    OP_REQUIRES_OK(context, RaggedComponentsFromVariant(
                                encoded_variant, input_ragged_rank_,
                                value_dtype, split_dtype, &decoded_components));

    // Corner case: input is a scalar.
    if (encoded_variant.dims() == 0) {
      ReturnRaggedTensor(context, decoded_components[0]);
      return;
    }

    // Nested-Stack Ragged components into a batched RaggedTensor.
    std::vector<int> encoded_dim_sizes(encoded_variant.dims(), 0);
    for (int i = 0; i < encoded_variant.dims(); i++) {
      encoded_dim_sizes[i] = encoded_variant.dim_size(i);
    }
    RaggedTensorVariant output_ragged;
    OP_REQUIRES_OK(
        context, NestedStackRaggedTensors<VALUE_TYPE, SPLIT_TYPE>(
                     decoded_components, encoded_dim_sizes, input_ragged_rank_,
                     output_ragged_rank_, &output_ragged));

    // Set output.
    ReturnRaggedTensor(context, output_ragged);
  }