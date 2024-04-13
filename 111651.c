  Status RunCallable(
      CallableHandle handle, const std::vector<Tensor>& feed_tensors,
      std::vector<Tensor>* fetch_tensors, RunMetadata* run_metadata,
      const thread::ThreadPoolOptions& threadpool_options) override {
    return wrapped_->RunCallable(handle, feed_tensors, fetch_tensors,
                                 run_metadata, threadpool_options);
  }