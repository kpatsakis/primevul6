  ~CudnnBatchNormAllocatorInOutput() override {
    if (!output_allocated) {
      Tensor* dummy_reserve_space = nullptr;
      OP_REQUIRES_OK(context_, context_->allocate_output(output_index_, {},
                                                         &dummy_reserve_space));
    }
  }