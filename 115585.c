  static bool check(Ope &ope) {
    IsPrioritizedChoice vis;
    ope.accept(vis);
    return vis.result_;
  }