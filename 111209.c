  explicit BinaryOp(OpKernelConstruction* ctx)
      : BinaryOpShared(ctx, DataTypeToEnum<Tout>::v(),
                       DataTypeToEnum<Tin>::v()) {}