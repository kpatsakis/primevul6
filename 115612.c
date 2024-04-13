  void operator=(F fn) {
    fn_ = make_adaptor(fn, fn);
  }