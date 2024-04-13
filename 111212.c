  void Left(const CPUDevice& d, typename Functor::tout_type out,
            typename Functor::tscalar_type scalar,
            typename Functor::tin_type in, bool* error) {
    typedef typename Functor::out_type Tout;
    typedef typename Functor::in_type Tin;
    typedef typename Functor::func Binary;
    typedef
        typename Eigen::internal::scalar_left<Tout, Tin, Binary,
                                              /*is_scalar_in_host_memory=*/true>
            Unary;
    Assign(d, out, in.unaryExpr(Unary(scalar.data())));
  }