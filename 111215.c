  void operator()(const CPUDevice& d, typename Functor::tout_type out,
                  typename Functor::tin_type in0,
                  typename Functor::tin_type in1, bool* error) {
    Assign(d, out, in0.binaryExpr(in1, typename Functor::func()));
  }