  CudnnBatchNormAllocatorInOutput(OpKernelContext* context, int output_index)
      : context_(context), output_index_(output_index) {}