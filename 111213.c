  void BCast(const CPUDevice& dev,
             typename TTypes<typename Functor::out_type, NDIMS>::Tensor out,
             typename TTypes<typename Functor::in_type, NDIMS>::ConstTensor in0,
             typename Eigen::array<Eigen::DenseIndex, NDIMS> bcast0,
             typename TTypes<typename Functor::in_type, NDIMS>::ConstTensor in1,
             typename Eigen::array<Eigen::DenseIndex, NDIMS> bcast1,
             bool* error) {
    typename Functor::func func;
    if (AllOne<NDIMS>(bcast0) && AllOne<NDIMS>(bcast1)) {
      Assign(dev, out, in0.binaryExpr(in1, func));
    } else if (AllOne<NDIMS>(bcast0)) {
      auto rhs = in1.broadcast(bcast1);
      Assign(dev, out, in0.binaryExpr(rhs, func));
    } else if (AllOne<NDIMS>(bcast1)) {
      auto lhs = in0.broadcast(bcast0);
      Assign(dev, out, lhs.binaryExpr(in1, func));
    } else {
      auto lhs = in0.broadcast(bcast0);
      auto rhs = in1.broadcast(bcast1);
      Assign(dev, out, lhs.binaryExpr(rhs, func));
    }
  }