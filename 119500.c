  bool IsIdentityPermutation(const std::vector<int64>& perm) {
    for (int64 i = 0, end = perm.size(); i < end; ++i) {
      if (i != perm[i]) {
        return false;
      }
    }
    return true;
  }