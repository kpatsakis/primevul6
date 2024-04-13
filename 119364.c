  bool IsInnerMatrixTranspose(const std::vector<T>& perm) {
    const T n = perm.size();
    if (n < 2) {
      return false;
    }
    for (T i = 0; i < n - 2; ++i) {
      if (perm[i] != i) {
        return false;
      }
    }
    return perm[n - 1] == n - 2 && perm[n - 2] == n - 1;
  }