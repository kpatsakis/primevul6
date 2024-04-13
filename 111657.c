  Status Run(const RunOptions& run_options,
             const std::vector<std::pair<string, Tensor>>& inputs,
             const std::vector<string>& output_tensor_names,
             const std::vector<string>& target_node_names,
             std::vector<Tensor>* outputs, RunMetadata* run_metadata) override {
    return wrapped_->Run(run_options, inputs, output_tensor_names,
                         target_node_names, outputs, run_metadata);
  }