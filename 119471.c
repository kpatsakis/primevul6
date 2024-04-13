  Status GetPermutation(const NodeDef& node_perm,
                        std::vector<int64>* perm64) const {
    std::vector<int> perm32;
    if (ValuesFromConstNode(node_perm, &perm32)) {
      perm64->reserve(perm32.size());
      for (int val : perm32) {
        perm64->push_back(static_cast<int64>(val));
      }
      return Status::OK();
    }
    if (ValuesFromConstNode(node_perm, perm64)) {
      return Status::OK();
    }
    return errors::InvalidArgument("Couldn't extract permutation from ",
                                   node_perm.name());
  }