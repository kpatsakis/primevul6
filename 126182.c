  explicit MergeV2Checkpoints(OpKernelConstruction* context)
      : OpKernel(context) {
    OP_REQUIRES_OK(context,
                   context->GetAttr("delete_old_dirs", &delete_old_dirs_));
  }