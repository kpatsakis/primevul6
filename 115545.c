  void visit(LiteralString &ope) override {
    if (ope.lit_.empty()) { set_error(); }
  }