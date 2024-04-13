  void ReturnRaggedTensor(OpKernelContext* context,
                          const RaggedTensorVariant& ragged_tensor) {
    int ragged_rank = ragged_tensor.ragged_rank();
    OpOutputList splits_out;
    OP_REQUIRES_OK(context,
                   context->output_list("output_nested_splits", &splits_out));
    for (int i = 0; i < ragged_rank; i++) {
      splits_out.set(i, ragged_tensor.splits(i));
    }
    context->set_output(ragged_rank, ragged_tensor.values());
  }