  void Right(const CPUDevice& d, typename Functor::tout_type out,
             typename Functor::tin_type in,
             typename Functor::tscalar_type scalar, bool* error) {
    typedef typename Functor::out_type Tout;
    typedef typename Functor::in_type Tin;
    typedef typename Functor::func Binary;
    typedef typename Eigen::internal::scalar_right<
        Tout, Tin, Binary, /*is_scalar_in_host_memory=*/true>
        Unary;
    Assign(d, out, in.unaryExpr(Unary(scalar.data())));
  }