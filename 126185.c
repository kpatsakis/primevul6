  void Compute(OpKernelContext* context) override {
    const Tensor& checkpoint_prefixes = context->input(0);
    const Tensor& destination_prefix = context->input(1);
    OP_REQUIRES(context,
                TensorShapeUtils::IsVector(checkpoint_prefixes.shape()),
                errors::InvalidArgument(
                    "Input checkpoint_prefixes should be an 1-D tensor, got ",
                    checkpoint_prefixes.shape().DebugString(), " instead."));
    OP_REQUIRES(context, TensorShapeUtils::IsScalar(destination_prefix.shape()),
                errors::InvalidArgument(
                    "Input destination_prefix should be a scalar tensor, got ",
                    destination_prefix.shape().DebugString(), " instead."));

    const gtl::ArraySlice<tstring> input_prefixes =
        gtl::ArraySlice<tstring>(checkpoint_prefixes.flat<tstring>());
    Env* env = Env::Default();
    const string& merged_prefix = destination_prefix.scalar<tstring>()();
    OP_REQUIRES_OK(
        context, tensorflow::MergeBundles(env, input_prefixes, merged_prefix));

    if (delete_old_dirs_) {
      const string merged_dir(io::Dirname(merged_prefix));
      for (const string& input_prefix : input_prefixes) {
        const string dirname(io::Dirname(input_prefix));
        if (dirname == merged_dir) continue;
        Status status = env->DeleteDir(dirname);
        // For sharded save, only the first delete will go through and all
        // others will hit NotFound.  Use vlog to be less verbose.
        if (!status.ok()) VLOG(1) << status;
      }
    }
  }