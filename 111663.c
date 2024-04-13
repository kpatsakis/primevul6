  Status Close(const RunOptions& run_options) override {
    return wrapped_->Close(run_options);
  }