  Status MakeCallable(const CallableOptions& callable_options,
                      CallableHandle* out_handle) override {
    return wrapped_->MakeCallable(callable_options, out_handle);
  }