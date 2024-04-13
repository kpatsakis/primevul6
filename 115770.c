  void Compute(OpKernelContext* context) override {
    FusedBatchNormGradOpBase<Device, T, U>::ComputeWithReservedSpace(context,
                                                                     false);
  }