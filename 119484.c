  bool FirstNInputsAreUnique(const NodeDef& node, int n) const {
    if (n > node.input_size()) return false;
    absl::flat_hash_set<string> unique_inputs;
    const int start = node.op() == "Concat" ? 1 : 0;
    const int end = start + n;
    for (int i = start; i < end; ++i) {
      unique_inputs.insert(node.input(i));
    }
    int unique_input_size = unique_inputs.size();
    return unique_input_size == n;
  }