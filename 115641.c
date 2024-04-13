  static bool check(Ope &ope) {
    IsLiteralToken vis;
    ope.accept(vis);
    return vis.result_;
  }