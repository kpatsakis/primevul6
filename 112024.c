  explicit QuantizedMaxPoolingOp(OpKernelConstruction* context)
      : MaxPoolingOp<Device, T>(context) {}