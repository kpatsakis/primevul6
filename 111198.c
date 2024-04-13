  inline Eigen::IndexList<int, Eigen::type2index<1>> NByOne(int n) {
    Eigen::IndexList<int, Eigen::type2index<1>> ret;
    ret.set(0, n);
    return ret;
  }