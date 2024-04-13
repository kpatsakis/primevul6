  bool AreInversePermutations(const std::vector<int64>& a,
                              const std::vector<int64>& b) {
    if (a.size() != b.size()) {
      return false;
    }
    for (int i = 0, end = a.size(); i < end; ++i) {
      if (a[b[i]] != i) {
        return false;
      }
    }
    return true;
  }