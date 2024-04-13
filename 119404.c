  bool IsSupported(const string& op_name, DataType dtype) const {
    const auto it = supported_ops_.find(op_name);
    return it != supported_ops_.end() && it->second.count(dtype) > 0;
  }