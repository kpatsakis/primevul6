  Status PRunSetup(const std::vector<string>& input_names,
                   const std::vector<string>& output_names,
                   const std::vector<string>& target_nodes,
                   string* handle) override {
    return errors::Unimplemented("Session::PRunSetup()");
  }