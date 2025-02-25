  void Compute(OpKernelContext* context) override {
    const Tensor& prefix = context->input(0);
    const Tensor& tensor_names = context->input(1);
    const Tensor& shape_and_slices = context->input(2);
    OP_REQUIRES(context, tensor_names.NumElements() == dtypes_.size(),
                errors::InvalidArgument("Got ", tensor_names.NumElements(),
                                        " tensor names, but ", dtypes_.size(),
                                        " expected dtypes."));
    ValidateInputs(false /* not save op */, context, prefix, tensor_names,
                   shape_and_slices);
    if (!context->status().ok()) return;

    const string& prefix_string = prefix.scalar<tstring>()();

    // Intention: we plan to use the RestoreV2 op as a backward-compatible
    // reader as we upgrade to the V2 format.  This allows transparent upgrade.
    // We here attempt to read a V1 checkpoint, if "prefix_string" does not
    // refer to a V2 checkpoint.
    Env* env = Env::Default();
    std::vector<string> paths;
    if (!env->GetMatchingPaths(MetaFilename(prefix_string), &paths).ok() ||
        paths.empty()) {
      // Cannot find V2's metadata file, so "prefix_string" does not point to a
      // V2 checkpoint.  Invokes the V1 read path instead.
      for (size_t i = 0; i < tensor_names.NumElements(); ++i) {
        RestoreTensor(context, &checkpoint::OpenTableTensorSliceReader,
                      /* preferred_shard */ -1, /* restore_slice */ true,
                      /* restore_index */ i);
        if (!context->status().ok()) {
          return;
        }
      }
      return;
    }
    // If found, invokes the V2 reader.
    OP_REQUIRES_OK(context, RestoreTensorsV2(context, prefix, tensor_names,
                                             shape_and_slices, dtypes_));
  }