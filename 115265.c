  void visit(LiteralString &ope) override {
    found_ope = ope.shared_from_this();
  }