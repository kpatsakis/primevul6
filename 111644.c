  Status RunCallable(CallableHandle handle,
                     const std::vector<Tensor>& feed_tensors,
                     std::vector<Tensor>* fetch_tensors,
                     RunMetadata* run_metadata) override {
    return wrapped_->RunCallable(handle, feed_tensors, fetch_tensors,
                                 run_metadata);
  }