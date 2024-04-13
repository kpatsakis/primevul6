  explicit FusedBatchNormOpEx(OpKernelConstruction* context)
      : FusedBatchNormOpBase<Device, T, U>(context,
                                           kWithSideInputAndActivation) {}