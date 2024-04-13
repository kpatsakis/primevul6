  Status PRun(const string& handle,
              const std::vector<std::pair<string, Tensor>>& inputs,
              const std::vector<string>& output_names,
              std::vector<Tensor>* outputs) override {
    return errors::Unimplemented("Session::PRun()");
  }