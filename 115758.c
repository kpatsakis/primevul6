  void Compute(OpKernelContext* context) override {
    FusedBatchNormOpBase<Device, T, U>::ComputeWithReservedSpace(context, true);
  }